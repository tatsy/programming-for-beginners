import os
import argparse

import cv2
import numpy as np
import pytesseract
import matplotlib.pyplot as plt
from PIL import Image
from sklearn.linear_model import LogisticRegression


def timer(func):
    """ Decorator to measure execution time """
    import time

    def wrapper(*args, **kwargs):
        start_time = time.time()
        ret = func(*args, **kwargs)
        elapsed = time.time() - start_time  
        print('{:s}: {:4f} sec'.format(func.__name__, elapsed))
        return ret

    return wrapper


def is_convex(pts):
    """ Check if the pts forms a convex polygon """
    pts = np.asarray(pts).reshape((-1, 2))
    n = len(pts)
    sign = 0
    for i in range(n):
        pre = (i - 1 + n) % n
        pst = (i + 1) % n
        e1 = pts[pre] - pts[i]
        e2 = pts[pst] - pts[i]
        det = e1[0] * e2[1] - e1[1] * e2[0]
        if sign == 0:
            sign = 1 if det >= 0 else -1
        else:
            if sign * det < 0:
                return False
            
    return True


@timer
def detect_edges(image, thresh1=100.0, thresh2=200.0):
    """ Detect consistent edges using Canny method """

    if image.ndim == 3:
        gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)
    elif image.ndim == 2:
        gray = image

    height, width = gray.shape
    struct_size = int(min(height, width) * 0.01)
    structure = np.ones((struct_size, struct_size), dtype=image.dtype)

    binary_image = cv2.Canny(gray, thresh1, thresh2)
    binary_image = cv2.morphologyEx(binary_image, cv2.MORPH_CLOSE, structure)

    return binary_image


@timer
def detect_frame(edge_image):
    """ Detect largest rectangle """

    # Detect contour
    maxlen = 0.0
    maxarc = None
    contours, _ = cv2.findContours(edge_image, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    # Search longest contour with 4 points
    for cnt in contours:
        arclen = cv2.arcLength(cnt, True)
        approx = cv2.approxPolyDP(cnt, 0.01 * arclen, True)
        
        # Restrict the search over convex polygons
        if maxlen < arclen and len(approx) == 4:
            if is_convex(approx):
                maxlen = arclen
                maxarc = approx
        
    frame = maxarc.reshape((-1, 2))
    return frame


@timer
def extract_sudoku(gray, src_pts):
    """ Extract sudoku region from input image """

    # Re-order points
    center = np.mean(src_pts, axis=1)
    pts = []
    for p in src_pts:
        dx = p[0] - center[0]
        dy = p[1] - center[1]
        theta = np.arctan2(dy, dx) + np.pi
        pts.append((theta, p))

    pts = sorted(pts, key=lambda p: p[0])
    pts = [p[1] for p in pts]

    # Unwarp the sudoku region
    src_pts = np.asarray(pts, dtype='float32')
    dst_pts = np.asarray([[0, 0], [900, 0], [900, 900], [0, 900]], dtype='float32')
    homography = cv2.getPerspectiveTransform(src_pts, dst_pts)
    sudoku_image = cv2.warpPerspective(gray, homography, (900, 900))

    return sudoku_image, homography


@timer
def image2text(sudoku_image, method='tesseract'):
    """ Convert sudoku image to text via OCR """

    # Check supported method
    supported_method = ['tesseract']
    if method not in supported_method:
        raise Exception('OCR method "{}" is not supported'.format(method))

    # Adaptive thresholding
    sudoku_image = cv2.adaptiveThreshold(sudoku_image, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C, cv2.THRESH_BINARY_INV, 31, 11)

    # OCR on characters in cells
    cells = np.zeros((9, 9), dtype='uint8')
    for i in range(9):
        for j in range(9):
            # Cut out character region
            xs = j * 100 + 15
            xe = (j + 1) * 100 - 15
            ys = i * 100 + 15
            ye = (i + 1) * 100 - 15
            cell = sudoku_image[ys:ye, xs:xe]

            # Align charecter to the center
            iy, ix = np.where(cell > 128)
            if ix.size > 0 and iy.size > 0:   
                xmin = np.min(ix)
                xmax = np.max(ix)
                ymin = np.min(iy)
                ymax = np.max(iy)
                cx = (xmin + xmax) // 2
                cy = (ymin + ymax) // 2

                h, w = cell.shape
                dx = w // 2 - cx
                dy = h // 2 - cy
                M = np.array([[1.0, 0.0, dx], [0.0, 1.0, dy]])
                cell = cv2.warpAffine(cell, M, (h, w))                
            
            cell = np.pad(cell, (5, 5), mode='constant', constant_values=0)

            if method == 'tesseract':
                config = '--psm 10 --oem 3 -c tessedit_char_whitelist=0123456789'
                cell = 255 - cell      
                ratio = np.count_nonzero(cell < 128) / cell.size

                # Run tesseract only for non-empty cells
                if ratio > 0.05:
                    text = pytesseract.image_to_string(Image.fromarray(cell), lang='eng', config=config)

                    if text != '':
                        cells[i, j] = int(text)

    return cells


def sudoku_solve_dfs(problem, start=0):
    """ Solve sudoku problem by DFS """    
    rows, cols = problem.shape
    nums = set([i for i in range(0, 10)])
    for index in range(start, rows * cols):
        i = index // cols
        j = index % cols
        if problem[i, j] == 0:
            k = i // 3
            l = j // 3
            row_nums = problem[i, :].tolist()
            col_nums = problem[:, j].tolist()
            blk_nums = problem[3*k:3*k+3, 3*l:3*l+3].reshape(-1).tolist()
            used = set(row_nums + col_nums + blk_nums)
            diff = nums.difference(used)
            if len(diff) == 0:
                return False
            
            success = False
            for n in diff:
                problem[i, j] = n
                if sudoku_solve_dfs(problem, i * cols + j):
                    success = True
                    break

                problem[i, j] = 0

            if not success:
                return False
        
    return True


@timer
def sudoku_solve(problem):
    """ Solve sudoku problem """
    answer = problem.copy()
    if sudoku_solve_dfs(answer):
        return answer
    
    return None


def main(filename):
    # Load input image
    image = cv2.imread(filename, cv2.IMREAD_COLOR)
    if image is None:
        raise Exception('Failed to load image file: ' + filename)

    # Resize too large image
    height, width, _ = image.shape
    if max(height, width) >= 1500:
        scale = 1500 / max(height, width)
        image = cv2.resize(image, None, fx=scale, fy=scale)
        height, width, _ = image.shape

    # Convert to color format
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    gray = cv2.cvtColor(image, cv2.COLOR_RGB2GRAY)

    # Detect edges
    edges = detect_edges(gray)

    # Detect largest rectangle
    frame_pts = detect_frame(edges)

    # Detect sudoku problem
    sudoku_image, homography = extract_sudoku(gray, frame_pts)
    problem = image2text(sudoku_image, method='tesseract')

    # Solve sudoku
    answer = sudoku_solve(problem)
    if answer is None:
        raise Exception('Failed to solve sudoku problem!')

    # Embed answer to original image
    overlay = np.zeros((900, 900, 4), dtype='float32')
    font = cv2.FONT_HERSHEY_PLAIN
    for i in range(9):
        for j in range(9):
            if problem[i, j] == 0:
                x = j * 100 + 20
                y = i * 100 + 100 - 20
                cv2.putText(overlay, str(answer[i, j]), (x, y), font, 5.0, (1, 0, 0, 1), 5, cv2.LINE_AA)

    warp_numbers = cv2.warpPerspective(overlay, np.linalg.inv(homography), (width, height))
    alpha = warp_numbers[:, :, 3:4]
    rgb = warp_numbers[:, :, :3]
    result = (1.0 - alpha) * (image / 255.0).astype('float32') + alpha * rgb

    plt.figure(figsize=(10, 10))
    plt.imshow(result)
    plt.axis('off')
    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Solve sudoku in photos')
    parser.add_argument('-i', '--input', type=str, required=True,
                        help='Input image')
    args = parser.parse_args()

    main(args.input)
