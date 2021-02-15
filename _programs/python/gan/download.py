import os
import sys
import gzip

import requests

curdir = os.path.abspath(os.path.dirname(__file__))

CHUNK_SIZE = 32768


def download(name, url, files):
    outdir = os.path.join(curdir, name)
    if not os.path.exists(outdir):
        os.makedirs(outdir)

    for f in files:
        # Download files
        session = requests.Session()
        response = session.get(os.path.join(url, f), stream=True)
        remote_file = os.path.join(url, f)
        local_file = os.path.join(outdir, f)
        print('Downloading: %s' % (remote_file))
        with open(local_file, 'wb') as fp:
            dl = 0
            for chunk in response.iter_content(CHUNK_SIZE):
                if chunk:
                    dl += len(chunk)
                    fp.write(chunk)

                    mb = dl / 1.0e6
                    sys.stdout.write('\r%.2f MB downloaded...' % (mb))
                    sys.stdout.flush()

            sys.stdout.write('\nFinish!\n')
            sys.stdout.flush()

        # Unzip
        unzip_file = os.path.splitext(local_file)[0]
        with open(unzip_file, 'wb') as fout, gzip.open(local_file, 'rb') as fin:
            data = fin.read()
            fout.write(data)

        os.remove(local_file)


def main():
    urls = [
        ('mnist', 'http://yann.lecun.com/exdb/mnist/'),
        ('kmnist', 'http://codh.rois.ac.jp/kmnist/dataset/kmnist/'),
    ]

    target = 0
    while target <= 0:
        for i, (name, url) in enumerate(urls):
            print('[%d] %s: %s' % (i + 1, name, url))
        target = input('Choose dataset to download: ')
        target = int(target)

    name, url = urls[target - 1]
    files = [
        'train-images-idx3-ubyte.gz',
        'train-labels-idx1-ubyte.gz',
        't10k-images-idx3-ubyte.gz',
        't10k-labels-idx1-ubyte.gz',
    ]
    download(name, url, files)


if __name__ == '__main__':
    main()
