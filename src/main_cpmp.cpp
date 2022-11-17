#include <map>
#include <vector>
#include <list>
#include <iterator>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <random>
#include <stdexcept>
#include<ctime>
#include "dirent.h"
#include "Layout.h"
#include "Greedy.h"
#include <queue>
#include <stack>

using namespace std;
using namespace cpmp;

int lb_counter=0;
    
class Nodo
{
    public:
        Layout * actual;
        Nodo * padre = NULL;
        int n_children=0;
        list < pair<int, int> > valid_moves;
        list < pair<int, int> > simulation;

        int nivel;
        bool greedy_child;
        bool selected = false;
        double score = 0;
        int ub = 100;
        
    
        Nodo(Layout * l, int lvl, Nodo * padre=NULL) : actual(new cpmp::Layout(*l)), 
        greedy_child(false), nivel(lvl), padre(padre)
        {
            
        }
    /*************************************************************/
    ///////////////////////Funciones///////////////////////////////
    /*************************************************************/

        ~Nodo()
        {
            delete(actual);
        }

        Nodo* next_child(int U, pair <int,int> default_action = make_pair(-1,-1) ){
            if(n_children == 0){
                int stacks = actual->stacks.size();
                int i,j;
                int h = actual->H;
                //Por cada stack
                for (i=0;i<stacks;i++)
                    for (j=0;j<stacks;j++){
                        if(default_action.first==i && default_action.second==j) continue;
                        //Si la columna actual no tiene tamaño 0 y Si la columna objetivo no esta llena
                        if (i != j && actual->stacks[i].size() != 0 && actual->stacks[j].size() != h && 
                                            actual->validate_move(i,j) &&  actual->validate_move2(i,j)){
                            int c = actual->stacks[i].back(); 
                            int top_dest = Layout::gvalue(actual->stacks[j]);

                            valid_moves.push_back( make_pair(i,j) );

                        }
                    }

                //std::random_device rd;
                //std::mt19937 generator(rd());
                //std::shuffle(valid_moves.begin(), valid_moves.end(), generator);  

                if (default_action.first!=-1) valid_moves.push_front(default_action);
            }

           
            while(valid_moves.size()>0){
                pair <int,int> move = valid_moves.front();
                valid_moves.pop_front();
                int i=move.first, j=move.second;

                int aux=actual->lb;
                actual->move(i,j,false);
                actual->lb2(); lb_counter++;
                int new_lb=actual->lb;
                actual->lb=aux; 
                actual->move(j,i,false); actual->steps-=2; 
               
            
                if (new_lb < U){
                    //Se crea un nuevo nodo
                    Nodo * niu = new Nodo(actual,(nivel)+1,this);
                    //Se realiza el movimiento
                    niu->actual->move(i,j,false);
                    niu->actual->lb = new_lb;

                    n_children++;
                    //Se retorna
                    return niu;
                }
            }

            return NULL;

        }

        void get_children(list <Nodo*> &children, int U)
        {
            int stacks = actual->stacks.size();
            int i,j;
            int h = actual->H;

            //Por cada stack
            for (i=0;i<stacks;i++)
            {
                //Se mueve al resto
                for (j=0;j<stacks;j++)
                {
                    //Si no es el mismo stack
                    //+
                    //Si la columna actual no tiene tamaño 0 y Si la columna objetivo no esta llena
                    if (i != j && actual->stacks[i].size() != 0 && actual->stacks[j].size() != h && 
                                        actual->validate_move(i,j) &&  actual->validate_move2(i,j))
                    {
                        int c = actual->stacks[i].back(); 
                        actual->move(i,j);
                        int aux=actual->lb;

                        actual->lb2(); lb_counter++;

                        actual->move(j,i); actual->steps-=2;
                        actual->seq.pop_front(); actual->seq.pop_front(); 

                        if (actual->lb < U){
                            //Se crea un nuevo nodo
                            Nodo * niu = new Nodo(actual,(nivel)+1,this);
                            //Se realiza el movimiento
                            niu->actual->move(i,j);
                            niu->actual->lb = actual->lb;

                            //Se inserta en LA COLA
                            children.push_back(niu);
                        }

                        actual->lb=aux;
                            
                    }
                }
            }
           // return pendientes;
        }

        void get_greedy_children(list <Nodo*> &children, int U, list < pair < int , pair <int, int> > > actions)
        {
            int i;
            for(i=0;i<actions.size();i++)
            {
                //Se crea un nuevo nodo
                Nodo * niu = new Nodo(actual,(nivel)+1,this);

                int ev = actions.front().first;
                pair<int,int> move= actions.front().second;

                //Se realiza el movimiento
                niu->actual->move(move.first,move.second, ev<0);
                niu->actual->lb = actual->lb;

                //Se inserta en la cola
                children.push_back(niu);
            }
        }
    //****************************************************************************
        
        
};

/*************************************************************/
//////////////Para comparar nodos según lb/////////////////////
/*************************************************************/

class compare_nodes2
{

public:
  bool operator() (const Nodo* lhs, const Nodo* rhs) const
  {
    if (lhs->actual->lb >= rhs->actual->lb) return (true);
    else return (false);
  }
};

class compare_nodes3
{

public:
  bool operator() (const Nodo* lhs, const Nodo* rhs) const
  {
    if (lhs->score >= rhs->score) return (true);
    else return (false);
  }
};

/*************************************************************/

class Tree
{
    public:
        Nodo * base;
        int limite;
        //stack <Nodo*> S;
        priority_queue<Nodo*, vector<Nodo*>, compare_nodes2> S;

        int nivel;
        Nodo * mejor;
        //Constructores
    

    /*************************************************************/
    ///////////////////////Funciones///////////////////////////////
    /*************************************************************/

    //Retorna la cantidad total de movimientos realizados
    static int greedy(Layout * L, int u=1000)
        {
            int steps;
            Layout h = *L;
            Layout * nuevo = new Layout(h);
            Layout sol = *nuevo;
            steps = greedy_solve(sol,u);
            
            delete(nuevo);
            
            return steps;
        }


    
    /*************************************************************/

    Tree(Layout * l, int lvl)
    {
        search2(l, lvl);
    }

    //A* con heuristica admisible lb2
    static void search2(Layout* l, int lvl)
    {
        lb_counter=0;
        map <int,int> lbs;
        Nodo* root = new Nodo(l,lvl,NULL);

        //Se calcula lb de la raiz
        root->actual->lb2(); lb_counter++;
        lbs[root->actual->lb]=1; 
        
        //Cola con prioridad para almacenar nodos
        priority_queue<Nodo*, vector<Nodo*>, compare_nodes2> S;
        S.push(root);
            
        int L = root->actual->lb;
        int U = greedy(root->actual);
        cout << "Greedy cost: " << U << endl;
        
        //Cantidad máxima de nodos creados
        int total_nodes = 0;
        //Contador de iteraciones
        int iter = 0;

        while (S.size()!=0)
        {
            //Se obtiene el elemento top del stack
            Nodo* temp = S.top(); S.pop();
            if(S.size() > total_nodes) total_nodes = S.size();

            int l = temp->actual->lb;
            lbs[l]--; if(lbs[l]==0) lbs.erase(l);
            
            if (l >= U){
                if (L == U){
                    cout << "Optimal cost: " << U << endl;
                    cout << "Total iterations: " << iter << endl;
                    cout << "Total nodes: " << total_nodes << endl;
                    return;
                }
                delete(temp);
                continue;
            }

            int u=1000;

            //Estado final
            if (temp->actual->unsorted_stacks==0) u=temp->actual->steps;
            
            if (u < U){
                U = u;
                //cout << "Current Upper-Bound: " << U << endl;
                if (l >= U){
                    delete(temp);
                    continue;
                }
            }
    
            if (L == U)
            {
                cout << "Optimal cost: " << U << endl;
                cout << "Total iterations: " << iter << endl;
                cout << "Total nodes: " << total_nodes << endl;
                return;
            }

            /*************************************************************/
            //////////////////IMPLEMENTACIÓN DE PROPUESTA//////////////////
            /*************************************************************/

            //Se crea la estructura de datos para los hijos y se obtienen estos
            list <Nodo*> children;
            temp->get_children(children, U);
            delete(temp);
            if (children.size()==0) continue;

            //Se guardan los hijos generados
            for (Nodo* aux:children){
                S.push(aux);
                lbs[aux->actual->lb]++;
            }

            //Se obtiene el mejor hijo
            Nodo* best_child = children.front();
            children.clear();

            //Se crea la lista de acciones
            list< pair<int, pair < int, int> > > actions;

            //Búsqueda Greedy
            while (true)
            {
                if (best_child->actual->unsorted_elements + best_child->actual->steps >= U) break;

                //Si se llegó a un estado final
                if(best_child->actual->unsorted_stacks==0)
                {
                    u = best_child->actual->steps;
                    if (u < U) U = u;
                    delete(best_child);
                    break;
                }

                Layout aux = *best_child->actual;
                Layout *layout = new Layout(aux);

                greedy_eval(*layout, actions);
                best_child->get_greedy_children(children, U, actions);

                best_child = children.front(); children.pop_front();

                // ---------- PARA GUARDAR EN LA COLA CON PRIORIDAD LOS HIJOS QUE GENERE LA BÚSQUEDA GREEDY ----------
                //for (Nodo* aux:children){
                //    S.push(aux);
                //    lbs[aux->actual->lb]++;
                //}
                children.clear();
                actions.clear();
            }

            //actualizar el lower bound
            L = lbs.begin()->first;
            //Aumentar contador de iteraciones
            iter++;
        }

        cout << "Optimal cost: " << U << endl;
        cout << "Total iterations: " << iter << endl;
        cout << "Total nodes: " << total_nodes << endl;

    }
     /*************************************************************/
};


int main(int argc, char * argv[]){
    printf("------- A* Greedy -------\n");
    cout << "Instance: " <<  argv[2] << endl;
    Layout::H = atoi (argv[1]);
    Layout L(argv[2]);

    Layout * nuevo = new Layout(L);

    const clock_t begin_tree = clock();
    Tree::search2(nuevo,0);

    cout << "Time elapsed: " << (float( clock () - begin_tree ) /  CLOCKS_PER_SEC) << "\n" << endl;
    return 0;
}