import gzip
import os
import sys

import requests

url = 'http://yann.lecun.com/exdb/mnist/'
x_train_file = 'train-images-idx3-ubyte.gz'
y_train_file = 'train-labels-idx1-ubyte.gz'
x_test_file = 't10k-images-idx3-ubyte.gz'
y_test_file = 't10k-labels-idx1-ubyte.gz'

curdir = os.path.abspath(os.path.dirname(__file__))
outdir = os.path.join(curdir, 'mnist')

CHUNK_SIZE = 32768


def main():
    if not os.path.exists(outdir):
        os.makedirs(outdir)

    files = [x_train_file, y_train_file, x_test_file, y_test_file]
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
        with open(unzip_file, 'wb') as fout, gzip.open(local_file,
                                                       'rb') as fin:
            data = fin.read()
            fout.write(data)


if __name__ == '__main__':
    main()
