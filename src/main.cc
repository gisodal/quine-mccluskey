#include "qm.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

using namespace std;

int main (int argc, char **argv){
    qm<uint32_t> q;

    opterr = 0;
    int i,c,index;
    char *opt;
    while ((c = getopt (argc, argv, "v:o:")) != -1){
        switch (c){
            case 'v':
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
                    fprintf (stderr, "Option -%c requires comma separated argument.\n", optopt);
                else if (optopt == 'v')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                return 1;
        }
    }
    for (index = optind; index < argc; index++)
        printf ("Non-option argument %s\n", argv[index]);

    if(q.valid())
        q.solve();
    else
        fprintf(stderr, "Input not valid, provide variables (-v #VARIABLES) and models (-o #models)\n");

    return 0;
}





//
//int main(){
//    qm q;
//
//
//using namespace std;
//
//int main(int argc, char** argv) {
//    int opt;
//    bool flagA = false;
//    bool flagB = false;
//
//    // Shut GetOpt error messages down (return '?'):
//    opterr = 0;
//
//    // Retrieve the options:
//    while ( (opt = getopt(argc, argv, "ab")) != -1 ) {  // for each option...
//        switch ( opt ) {
//            case 'a':
//                    flagA = true;
//                break;
//            case 'b':
//                    flagB = true;
//                break;
//            case '?':  // unknown option...
//                    cerr << "Unknown option: '" << char(optopt) << "'!" << endl;
//                break;
//        }
//    }
//
//    // Debug:
//    cout << "flagA = " << flagA << endl;
//    cout << "flagB = " << flagB << endl;
//
//    return 0;
//}
//
//
//
//
//
//
//
//    q.add_variable(0);
//    q.add_variable(1);
//    q.add_variable(2);
//    q.add_model(0);
//    q.add_model(1);
//    q.add_model(3);
//    q.add_model(4);
//    q.add_model(6);
//    q.add_model(7);
//    q.compute_primes();
//}
