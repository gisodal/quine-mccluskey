#include "qm.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

using namespace std;
#if __LP64__
const int MAX_QM = 128;
typedef uint128_t T;
#else
const int MAX_QM = 64;
typedef uint64_t T;
#endif

int main (int argc, char **argv){
    qm<T> q;

    opterr = 0;
    int i,c,index;
    char *opt;
    while ((c = getopt (argc, argv, "v:o:")) != -1){
        switch (c){
            case 'v':
                if(atoi(optarg) > MAX_QM){
                    fprintf(stderr, "Cannot handle more than %d variables\n", MAX_QM);
                    exit(1);
                }

                for(i = 0; i < atoi(optarg); i++)
                    q.add_variable(i);
                break;
            case 'o':
                opt = strtok (optarg,",");
                while(opt != NULL){
                    q.add_model(atoi(opt));
                    opt = strtok (NULL, ",");
                }
                break;
            case '?':
                if (optopt == 'o')
                    fprintf (stderr, "Option -%c requires comma separated list of models.\n", optopt);
                else if (optopt == 'v')
                    fprintf (stderr, "Option -%c requires #variables as argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);

                fprintf (stderr, "Usage: bin/quine-mccluskey -v <#variables> -o <model1[,model2[,...]]>\n");
                return 1;
            default:
                    fprintf (stderr, "Usage: bin/quine-mccluskey -v <#variables> -o <model1[,model2[,...]]>\n");
                return 1;
        }
    }
    for (index = optind; index < argc; index++)
        fprintf(stderr, "Ingoring argument '%s'\n", argv[index]);

    q.solve();
    if(q.reduced())
        printf("reduced!\n");
    q.print();

    return 0;
}

