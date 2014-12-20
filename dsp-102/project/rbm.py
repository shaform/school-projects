import numpy
import scipy.io

class RBM(object):
    def __init__(self, vnum, hnum):
        self.vnum = vnum
        self.hnum = hnum

        self.weights = numpy.random.uniform(-0.01, 0.01, (vnum, hnum))
        self.vbias = numpy.zeros(vnum)
        self.hbias = numpy.zeros(hnum)

    def sample_binary(self, means):
        # samples[i] = 1 with prob: means[i]
        samples = numpy.zeros(means.shape)
        probs = numpy.random.uniform(size=means.shape)
        samples[probs < means] = 1.0
        return samples

    def ph(self, vis):
        ''' p(h|v) = sigmoid(hbias + Wv) '''
        return self.sigmoid(numpy.dot(vis, self.weights) + self.hbias)

    def pv(self, hid):
        ''' p(v|h) = sigmoid(vbias + W'h) '''
        return self.sigmoid(numpy.dot(hid, self.weights.T) + self.vbias)

    def sigmoid(self, x):
        ''' s(x) = 1/(1+e^(-x)) '''
        return 1/(1+numpy.exp(-x))


class Trainer(object):
    def __init__(self, rbm, rate=0.001):
        self.rbm = rbm
        self.rate = rate

    def train(self, training_data, iterations, batch_size=50):
        for it in range(iterations):
            for i in range(0, len(training_data), batch_size):
                x_i = training_data[i:i+batch_size]
                bsize = x_i.shape[0]

                # gibbs sampling
                p_j = self.rbm.ph(x_i)
                y_j = self.rbm.sample_binary(p_j)
                p_i = self.rbm.pv(y_j)
                p_j_ = self.rbm.ph(p_i)

                # update parameters
                self.rbm.weights += self.rate * (numpy.dot(x_i.T, p_j) - numpy.dot(p_i.T, p_j_)) / bsize
                self.rbm.hbias += self.rate * (p_j.sum(axis=0) - p_j_.sum(axis=0)) / bsize
                self.rbm.vbias += self.rate * ((x_i.sum(axis=0) - p_i.sum(axis=0)) / bsize)

            print 'Iteration #%d finished' % (it + 1)

class Tester(object):
    ISIZE, OSIZE = 784, 500

    def __init__(self):
        mfile = scipy.io.loadmat('mnist_all.mat')
        self.data = mfile['train0']
        self.rbm = RBM(Tester.ISIZE, Tester.OSIZE)
        self.trainer = Trainer(self.rbm)

    def test(self):
        self.trainer.train(self.data, 10)

if __name__ == '__main__':
    tester = Tester()
    tester.test()
