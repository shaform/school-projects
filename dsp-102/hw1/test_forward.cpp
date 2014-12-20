#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include "hmm.h"

using std::make_pair;
using std::max;
using std::pair;
using std::vector;

enum Method {NAIVE, VITERBI, FORWARD};
const Method DEFAULT_METHOD = FORWARD;
const int MN = 5; // maximum number of models
const int FLEN = 20; // maximum length of model name

static void usage(char *argv[]) {
    printf("Usage : %s MODEL_LIST TEST_DATA RESULT [METHOD [COMPARE [-i n]]] \n"
            "\n\nMETHOD: viterbi, naive\n"
            "COMPARE: answer to compare\n"
            "-i: compare different iterations\n",
            argv[0]);
    exit(1);
}

pair<int, double> naive(HMM hmm[], const int mn, const char *seq);
pair<int, double> viterbi(HMM hmm[], const int mn, const char *seq);
pair<int, double> forward(HMM hmm[], const int mn, const char *seq);
void test_model(HMM *hmm, int mn, Method method, FILE *finput, FILE *foutput, FILE *fcompare, int iter_num = 0);

int main(int argc, char *argv[]) {
    if (argc < 4) {
        usage(argv);
    }
    // open input/output files
    FILE *finput, *foutput;
    finput = fopen(argv[2], "r");
    foutput = fopen(argv[3], "w");
    if (finput == NULL || foutput == NULL) {
        printf("cannot open files\n");
        usage(argv);
    }
    // detect method
    Method method = DEFAULT_METHOD;
    if (argc >= 5) {
        if (strcmp(argv[4], "naive") == 0) {
            method = NAIVE;
        } else if (strcmp(argv[4], "forward") == 0) {
            method = FORWARD;
        } else if (strcmp(argv[4], "viterbi") == 0) {
            method = VITERBI;
        }
    }
    // open compare file
    FILE *fcompare = NULL;
    int compare_iter = 0;
    if (argc >= 6) {  // if there is a compare file
        fcompare = fopen(argv[5], "r");
        if (fcompare == NULL) {
            printf("cannot open compare file\n");
        }
        if (argc >= 8) {  // if iteration compare is enabled
            if (sscanf(argv[7], "%d", &compare_iter) != 1 || compare_iter <= 1) {
                printf("invalid iteration number\n");
                compare_iter = 0;
            }
        }
    }

    // load models
    HMM hmm[MN];
    if (compare_iter) {  // load each iteration
        for (int i=1; i<=compare_iter; ++i) {
            const int mn = load_models(argv[1], hmm, MN, i);
            test_model(hmm, mn, method, finput, foutput, fcompare, i);
            fseek(finput, 0, SEEK_SET);
            fseek(fcompare, 0, SEEK_SET);
        }
    } else {  // load only one
        const int mn = load_models(argv[1], hmm, MN);
        test_model(hmm, mn, method, finput, foutput, fcompare);
    }


    fclose(finput);
    fclose(foutput);
    if (fcompare) {
        fclose(fcompare);
    }

    return 0;
}

void test_model(HMM *hmm, int mn, Method method, FILE *finput, FILE *foutput, FILE *fcompare, int iter_num) {

    // load test data
    char seq[T+1];
    int correct_num = 0, total_num = 0;
    while (fscanf(finput, "%s\n", seq) == 1) {
        // convert chars to observations
        for (int i=0; seq[i] != '\0'; ++i) {
            seq[i] -= 'A';
        }

        pair<int, double> max_model;
        if (method == FORWARD) {
            max_model = forward(hmm, mn, seq);
        } else if (method == NAIVE) {
            max_model = naive(hmm, mn, seq);
        } else {
            max_model = viterbi(hmm, mn, seq);
        }

        // only generate output when not testing iterations
        if (iter_num == 0) {
            fprintf(foutput, "%s %e\n", hmm[max_model.first].model_name, max_model.second);
        }

        if (fcompare) {
            char compare_model_name[FLEN+1];
            fscanf(fcompare, "%s", compare_model_name);
            if (strcmp(compare_model_name, hmm[max_model.first].model_name) == 0) {
                ++correct_num;
            }
            ++total_num;
        }
    }

    if (fcompare) {
        printf("%d: %f\n", iter_num, static_cast<double>(correct_num) / total_num);
    }
}

pair<int, double> naive(HMM hmm[], const int mn, const char *seq) {
    double alpha[T][MAX_STATE], beta[T][MAX_STATE], gamma[T][MAX_STATE],
           pol;
    double max_prob = 0;  // the probability of the most likely model
    int max_model = 0;  // the most likely model

    // initialize beta[T-1][], these values are fixed
    for (int s=0; s<MAX_STATE; ++s) {
        beta[T-1][s] = 1.0;
    }

    for (int i=0; i<mn; ++i) {
        const int N = hmm[i].state_num;

        // initialize alpha[0][], these values depend on the given model
        for (int s=0; s<N; ++s) {
            alpha[0][s] = hmm[i].initial[s] * hmm[i].observation[seq[0]][s];
        }

        // calculate alpha
        for (int t=1; t<T; ++t) {
            for (int s=0; s<N; ++s) {
                alpha[t][s] = 0.0;
                for (int k=0; k<N; ++k) {
                    alpha[t][s] += alpha[t-1][k] * hmm[i].transition[k][s];
                }
                alpha[t][s] *= hmm[i].observation[seq[t]][s];
            }
        }
        // calculate beta
        for (int t=T-2; t>=0; --t) {
            for (int s=0; s<N; ++s) {
                beta[t][s] = 0.0;
                for (int k=0; k<N; ++k) {
                    beta[t][s] += hmm[i].transition[s][k] * hmm[i].observation[seq[t+1]][k] * beta[t+1][k];
                }
            }
        }
        // calculate gamma
        pol = 0.0;
        for (int s=0; s<N; ++s) {
            pol += alpha[T-1][s];
        }
        for (int t=0; t<T; ++t) {
            for (int s=0; s<N; ++s) {
                gamma[t][s] = alpha[t][s] * beta[t][s] / pol;
            }
        }
        // calculate likelihood of this model
        double lmax_prob = 1;
        for (int t=0; t<T; ++t) {
            double tmax_prob = 0;
            for (int s=0; s<N; ++s) {
                if (gamma[t][s] > tmax_prob) {
                    tmax_prob = gamma[t][s];
                }
            }
            lmax_prob *= tmax_prob;
        }

        // check if this model is more likely
        if (lmax_prob > max_prob) {
            max_prob = lmax_prob;
            max_model = i;
        }
    }

    return make_pair(max_model, max_prob);
}

pair<int, double> viterbi(HMM hmm[], const int mn, const char *seq) {
    double delta[T][MAX_STATE];
    double max_prob = 0;  // the probability of the most likely model
    int max_model = 0;  // the most likely model

    for (int i=0; i<mn; ++i) {
        const int N = hmm[i].state_num;

        // initialize delta[0][]
        for (int s=0; s<N; ++s) {
            delta[0][s] = hmm[i].initial[s] * hmm[i].observation[seq[0]][s];
        }

        // calculate delta
        for (int t=1; t<T; ++t) {
            for (int s=0; s<N; ++s) {
                delta[t][s] = 0;
                for (int r=0; r<N; ++r) {
                    double ndelta = delta[t-1][r] * hmm[i].transition[r][s]
                        * hmm[i].observation[seq[t]][s];
                    delta[t][s] = max(ndelta, delta[t][s]);
                }
            }
        }

        // calculate likelihood of this model
        double lmax_prob = 0;
        for (int s=0; s<N; ++s) {
            if (delta[T-1][s] > lmax_prob) {
                lmax_prob = delta[T-1][s];
            }
        }

        // check if this model is more likely
        if (lmax_prob > max_prob) {
            max_prob = lmax_prob;
            max_model = i;
        }
    }

    return make_pair(max_model, max_prob);
}

pair<int, double> forward(HMM hmm[], const int mn, const char *seq) {
    double alpha[T][MAX_STATE], pol;
    double max_prob = 0;  // the probability of the most likely model
    int max_model = 0;  // the most likely model

    for (int i=0; i<mn; ++i) {
        const int N = hmm[i].state_num;

        // calculate alpha
        for (int s=0; s<N; ++s) {
            alpha[0][s] = hmm[i].initial[s] * hmm[i].observation[seq[0]][s];
        }
        for (int t=1; t<T; ++t) {
            for (int s=0; s<N; ++s) {
                alpha[t][s] = 0.0;
                for (int k=0; k<N; ++k) {
                    alpha[t][s] += alpha[t-1][k] * hmm[i].transition[k][s];
                }
                alpha[t][s] *= hmm[i].observation[seq[t]][s];
            }
        }
        pol = 0.0;
        for (int s=0; s<N; ++s) {
            pol += alpha[T-1][s];
        }

        // check if this model is more likely
        if (pol > max_prob) {
            max_prob = pol;
            max_model = i;
        }
    }

    return make_pair(max_model, max_prob);
}
