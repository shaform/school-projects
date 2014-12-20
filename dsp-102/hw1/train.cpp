#include <cstdio>
#include <cstdlib>
#include <sstream>
#include "hmm.h"

using std::ostringstream;

static void usage(char *argv[]) {
    printf("Usage : %s ITER INIT_MODEL INPUT_SEQ OUTPUT_MODEL [-a]\n"
            "\n-a\toutput 1~n iterations as OUTPUT_MODEL.1~n\n",
            argv[0]);
    exit(1);
}

void forward_backward(HMM *hmm, FILE *observ);

int main(int argc, char *argv[]) {
    if (argc < 5) {
        usage(argv);
    }
    int iter;
    if (sscanf(argv[1], "%d", &iter) != 1) {
        printf("invalid ITER\n");
        usage(argv);
    }
    FILE *finput, *foutput;
    finput = fopen(argv[3], "r");
    foutput = fopen(argv[4], "w");
    if (finput == NULL || foutput == NULL) {
        printf("cannot open files\n");
        usage(argv);
    }
    const bool all_iters = argc == 6 && strcmp(argv[5], "-a") == 0;


    // load initial model
    HMM hmm;
    loadHMM(&hmm, argv[2]);

    // train
    for (int i=0; i<iter; ++i) {
        forward_backward(&hmm, finput);
        if (all_iters) {
            ostringstream oss;
            oss << argv[4] << "." << i+1;
            FILE *foutput_iter = fopen(oss.str().c_str(), "w");
            if (foutput_iter) {
                dumpHMM(foutput_iter, &hmm);
                fclose(foutput_iter);
            }
        }
        fseek(finput, 0, SEEK_SET);
    }

    // dump
    dumpHMM(foutput, &hmm);

    fclose(finput);
    fclose(foutput);
    return 0;
}



void forward_backward(HMM *hmm, FILE *finput) {

    const int N = hmm->state_num;
    char seq[T+1];
    double alpha[T][MAX_STATE], beta[T][MAX_STATE], gamma[T][MAX_STATE],
           pol, epsilon[T-1][MAX_STATE][MAX_STATE];
    double new_initial[MAX_STATE],
           sum_epsilon[MAX_STATE][MAX_STATE],
           sum_gamma_n1[MAX_STATE], sum_gamma[MAX_STATE], sum_gamma_observ[MAX_STATE][NOB];

    // initialize beta[T-1][], these values are fixed
    for (int s=0; s<N; ++s) {
        beta[T-1][s] = 1.0;
    }

    memset(new_initial, 0, sizeof(new_initial));
    memset(sum_epsilon, 0, sizeof(sum_epsilon));
    memset(sum_gamma_n1, 0, sizeof(sum_gamma_n1));
    memset(sum_gamma, 0, sizeof(sum_gamma));
    memset(sum_gamma_observ, 0, sizeof(sum_gamma_observ));

    // process sequences one by one
    int seq_num = 0;
    while (fscanf(finput, "%s", seq) == 1) {
        // convert chars to observations
        for (int i=0; seq[i] != '\0'; ++i) {
            seq[i] -= 'A';
        }


        // === E-step === //


        // initialize alpha[0][], these values depend on the given sequence
        for (int s=0; s<N; ++s) {
            alpha[0][s] = hmm->initial[s] * hmm->observation[seq[0]][s];
        }

        // calculate alpha
        for (int t=1; t<T; ++t) {
            for (int s=0; s<N; ++s) {
                alpha[t][s] = 0.0;
                for (int k=0; k<N; ++k) {
                    alpha[t][s] += alpha[t-1][k] * hmm->transition[k][s];
                }
                alpha[t][s] *= hmm->observation[seq[t]][s];
            }
        }
        // calculate beta
        for (int t=T-2; t>=0; --t) {
            for (int s=0; s<N; ++s) {
                beta[t][s] = 0.0;
                for (int k=0; k<N; ++k) {
                    beta[t][s] += hmm->transition[s][k] * hmm->observation[seq[t+1]][k] * beta[t+1][k];
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
        // calculate epsilon
        for (int t=0; t<T-1; ++t) {
            for (int i=0; i<N; ++i) {
                for (int j=0; j<N; ++j) {
                    epsilon[t][i][j] = alpha[t][i] * hmm->transition[i][j]
                        * hmm->observation[seq[t+1]][j] * beta[t+1][j] / pol;
                }
            }
        }



        // === M-step === //
        for (int s=0; s<N; ++s) {
            new_initial[s] += gamma[0][s];
        }
        for (int i=0; i<N; ++i) {
            for (int j=0; j<N; ++j) {
                for (int t=0; t<T-1; ++t) {
                    sum_epsilon[i][j] += epsilon[t][i][j];
                }
            }
        }
        for (int s=0; s<N; ++s) {
            double lsum_gamma = 0.0;
            for (int t=0; t<T-1; ++t) {
                lsum_gamma += gamma[t][s];
            }
            sum_gamma_n1[s] += lsum_gamma;
            sum_gamma[s] += lsum_gamma + gamma[T-1][s];
        }
        for (int t=0; t<T; ++t) {
            for (int ob=0; ob<NOB; ++ob) {
                if (seq[t] == ob) {
                    for (int s=0; s<N; ++s) {
                        sum_gamma_observ[s][ob] += gamma[t][s];
                    }
                }
            }
        }

        ++seq_num;
    }


    // === M-step === //
    for (int s=0; s<N; ++s) {
        hmm->initial[s] = new_initial[s] / seq_num;
        for (int j=0; j<N; ++j) {
            hmm->transition[s][j] = sum_epsilon[s][j] / sum_gamma_n1[s];
        }
        for (int ob=0; ob<NOB; ++ob) {
            hmm->observation[ob][s] = sum_gamma_observ[s][ob] / sum_gamma[s];
        }
    }
}
