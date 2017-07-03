#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

#include <sys/time.h>
#include <time.h>


long long count=1;

struct BST_OPTI {
  long long cost;
  long long root;
};

typedef struct BST_OPTI bst_opti;

struct NODE {
  long long fg,fd;
};

typedef struct NODE node;

/**
 * \brief Implements the Bellman equation, searching for an optimal BST
 * \param p  A matrix that will contain the optimal roots and costs
 * \param probas  An array containing the sum of the frequencies up to a symbol
 * \param n The length of the array
 * \returns { updates the parameter p with optimal roots and costs }
 */
void optimize(bst_opti** p, long long* probas, long n) {
  long k;
  long q;
  long i;
  for (q = 1; q < n; q++) {
    for (i = 0; i < n-q; i++) {
      k = i+q;
      long long tmp;
      long long minimum=p[i][i].cost+p[i+1][k].cost;
      long long roo=-2;
      long long flag;
      for (tmp = p[i][k-1].root; tmp < p[i+1][k].root+1; tmp++) {
	  if (tmp == i) {
	    flag = p[tmp+1][k].cost;
	    }
	  else if (tmp == k) {
	    flag = p[i][tmp-1].cost;
	    }
	  else {
	    flag = p[i][tmp-1].cost+p[tmp+1][k].cost;
	    }
	  if (flag < minimum) {
	    minimum = flag;
	    roo = tmp;
	    }
	}
      p[i][k].cost = probas[k+1] - probas[i] + minimum;
      p[i][k].root = roo;
    }
    }
}

/**
 * \brief Constructs an array representing an optimal BST
 * \param p  A matrix containing the optimal roots and costs
 * \param tab_roots[]  An array that will contain a representation of the optimal BST
 * \param a  The index of the first element
 * \param b  The index of the last element
 * \param index  The index of the current root
 * \param length  The height of the matrix
 * \returns { returns nothing but fills the tab_roots parameter }
 */
void rec(bst_opti** p, node tab_roots[], long long a, long long b, long long index, long long length) {
	long long fg, fd;
	if (count < 1) return;
	count--;
        
	if (index > a && index < b) {
	    fg = p[a][index-1].root;
	    fd = p[index+1][b].root;
	    tab_roots[index].fg=fg;
	    tab_roots[index].fd=fd;
	    rec(p, tab_roots, a, index-1, fg, length);
	    rec(p, tab_roots, index+1, b, fd, length);
	  }
	
	else if (index <= a && index < b) {
	    fd = p[index+1][b].root;
	    tab_roots[index].fd=fd;
	    rec(p, tab_roots, index+1, b, fd, length);
	  }
	
	else if (index > a && index >= b) {
	    fg = p[a][index-1].root;
	    tab_roots[index].fg=fg;
	    rec(p, tab_roots, a, index-1, fg, length);
	  }
	else
	  return;
      }

/**
 * Displays the optimal BST
 * \brief Displays the optimal BST
 * \param tab_roots[]  An array representing the BST
 * \param length  The length ot the array
 * \returns { prints C code implementing the optimal BST }
 */
void print_roots(node tab_roots[], long long length) {
  long long i;
  for (i = 0; i < length-1; i++) {
      printf("{%lli, %lli},\n",tab_roots[i].fg,tab_roots[i].fd);
    }
  printf("{%lli, %lli}",tab_roots[length-1].fg, tab_roots[length-1].fd);
}

/**
 * Main function
 * \brief Main function
 * \param argc  A count of the number of command-line arguments
 * \param argv  An argument vector of the command-line arguments.
 * \warning Must be called with a positive integer, n,  and a filename, freqFile, as commandline parameters and in the given order.
 * \returns { 0 if succeeds and prints C code implementing an optimal ABR on stdout; exit code otherwise}
 */
int main (int argc, char *argv[]) {
  long n = 0 ; // Number of elements in the dictionary
  FILE *freqFile = NULL ; // File that contains n positive longegers defining the relative frequence of dictinary elements 
  
  if(argc != 3){
    fprintf(stderr, "!!!!! Usage: ./compileBST n  originalFile !!!!!\n");
      exit(EXIT_FAILURE); /* indicate failure.*/
  }

  { // Conversion of parameter n in a long 
    int codeRetour = EXIT_SUCCESS;
    char *posError;
    long resuLong;
    n = atol(argv[1] ) ;
   
    assert(argc >= 2);
    // Conversion of argv[1] en long
    resuLong = strtol(argv[1], &posError, 10);
    // Traitement des erreurs
    switch (errno)
    {
      case EXIT_SUCCESS :
         // Conversion du int en long
         if (resuLong > 0)
         {
            n = (long)resuLong;
            //fprintf(stderr, "Number of elements in the dictionary: %ld\n", n);         
         }
         else
         {
            (void)fprintf(stderr, "%s cannot be converted into a positive integer matching the number of elements in the dicionary.\n", argv[1]) ; 
            codeRetour = EXIT_FAILURE;
         }
      break;
      
      case EINVAL :
         perror(__func__);
         (void)fprintf(stderr, "%s does not match a long integer value. \n", argv[1]);
         codeRetour = EXIT_FAILURE;
      break;
      
      case ERANGE :
         perror(__func__);
         (void)fprintf(stderr, "%s does not fit in a long int :\n" "out of bound [%ld;%ld]\n",
                       argv[1], LONG_MIN, LONG_MAX);
         codeRetour = EXIT_FAILURE;
      break;
      default :
         perror(__func__);
         codeRetour = EXIT_FAILURE;
    } // switch (errno)
    if  (codeRetour != EXIT_SUCCESS) return codeRetour ;
  }

  freqFile = fopen(argv[2] , "r" );
  if (freqFile==NULL) {fprintf (stderr, "!!!!! Error opening originalFile !!!!!\n"); exit(EXIT_FAILURE);}

  // Timer
  
  //clock_t start, end;
  //double cpu_time_used;
  //start = clock();
  
  long long freqTab[n];
  long long i;
  for (i = 0; i < n ; i++) {
      fscanf(freqFile,"%lli",freqTab+i);
    }


  /* Allocation de la matrice de coûts et initialisation à -1 et à Pk pour la case d'indice (k,k) 
     (cas où l'arbre n'a qu'un élément, une racine, optimale a fortiori)
  */

  bst_opti** p;
  p = calloc(n,sizeof(*p));
  if (p == NULL) printf("Erreur d'allocation");
  else
    for (i = 0; i < n; i++) {
      p[i] = calloc(n, sizeof(**p));
      if (p == NULL) printf("Erreur d'allocation");
      p[i][i].cost = freqTab[i];
      p[i][i].root = i;
      }


  long long pnew[n+1];
  pnew[0] = 0;
  for (i = 1 ; i < n+1 ; i++)
    pnew[i] = pnew[i-1] + freqTab[i-1];

  
  /* Remplissage de la matrice principale, p[0][n-1].cost contiendra le coût de l'arbre optimal contenant les éléments e1,...,en
     et p[0][n-1].root sera la racine de cet arbre optimal
  */

  optimize(p, pnew, n);

  node tab_roots[n];
  // Ensuite, on procède à la construction récursive de l'arbre, en appelant la fonction rec définie plus haut
  for (i = 0; i < n; i++) {
      tab_roots[i].fg = -1;
      tab_roots[i].fd = -1;
    }

  count = n-1;
  rec(p, tab_roots, 0, n-1, p[0][n-1].root, n);
      
      
  // Affichage du resultat sur le terminal
  printf("static int BSTroot = %lli;\n",p[0][n-1].root);
  printf("static int BSTtree[%li][2] = {\n",n);
  print_roots(tab_roots,n);
  printf(" };\n");



  // Destruction de la matrice de couts
  for (i=0; i<n; i++) free(p[i]);
  free(p); 

  //end = clock();
  //cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  //printf("temps ecoule en secondes : %lf\n",cpu_time_used);

  
  fclose(freqFile);
  

  return 0;
}
