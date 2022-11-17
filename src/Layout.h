
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <list>
#include <iterator>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <unordered_map>
#include <functional> //for std::hash
#include <typeinfo> // para testing
#include <algorithm>


#ifndef _LAYOUT_
#define _LAYOUT_

using namespace std;


namespace cpmp { 

class Layout {
    public:
        deque <deque <int>> stacks;

        vector<int> sorted_elements; /*for each stack*/
        int total_elements;
        int unsorted_stacks = 0;
        int unsorted_elements = 0;
        int steps = 0;
        int full_stacks = 0;
        int lb;
        list <int> bsg_moves;
        set <int> dismantled_stacks;
        list < pair<int,int> > seq;
        set <int, std::greater<int>> gvalues;

        //Para la reducción de stacks
        int dismantling_stack = -1;  //stack que se está reduciendo
        map< int, int > assignation;
        set<int> blocked_stacks;

        list < pair <int, int> > moves;

        //multimap <int ,int> gv2index; //for the beamsearch (type:SDPP)
        int last_sd =-1; /*last dismantled stack*/

        static int H;

    public:

        Layout(string filename);

        int move(int i, int j, bool reducing=false, bool seqq=true);

        //Return false if it detect an unrelated move symmetry
        bool validate_move(int s1, int s2){
            if (s1 < s2) return true;

            int h1= stacks[s1].size(); int h10=h1;
            int h2= stacks[s2].size(); int h20=h2;

            //se desea saber si el mov: s1->s2 se puede realizar antes,
            //si es así: return False
            for(auto ss : seq){
                int so = ss.first;
                int sd = ss.second;
                if (so==s1) h1+=1; else if (sd==s1) h1-=1;
                if (so==s2) h2+=1; else if (sd==s2) h2-=1;
                if (h2==H) return true;
                if (h2<h20 || h1<h10) return true; //stacks variantes
                if (h20==h2 && h10==h1) return false; //stacks invariantes
            }
            return true; //first move
        }

        bool validate_move2(int st, int sd){
            vector<int> h(stacks.size());
            for (int i=0; i<stacks.size();i++) h[i]=stacks[i].size();
            vector<bool> variant_stacks(stacks.size(), false);

            for(auto ss : seq){
                int so = ss.first;
                int sdd = ss.second;
                
                if (sdd==st && !variant_stacks[so]){
                    if (stacks[so].size()==h[so] && stacks[st].size()==h[st] && stacks[sd].size()==h[sd]) 
                        return false;
                }
                h[so]+=1; h[sdd]-=1;
                if (h[sdd]<stacks[sdd].size()){ //variant stack
                    if (sdd==st || sdd==sd) return true;
                    variant_stacks[sdd]=true;
                }
            }
            return true; //first move
        }



        bool is_sorted(int j) const{
            return (stacks[j].size() == sorted_elements[j]);
        }

        // Optimal: O(1)
        int capacity(int s_o, int s_d=-1){
            int n=1;
            int len=stacks[s_o].size();
            if(s_d!=-1) {len+=stacks[s_d].size(); n++;}

            return ( (Layout::H) * (size()-n) - (total_elements-len) );
        }

        int sum_stack(int i) const{
            int sum =0;
            for(int c : stacks[i]) sum+=c;
            return sum;
        }

        //reachable height of stack i by only performing BG moves
        int reachable_height(int i) const;

        int size() const {return stacks.size();}

        int n_unblocked(int s);

        static int compute_sorted_elements(deque<int>& stack){
            if (stack.size()==0) return 0;
            int sorted_elements=1;
            while(sorted_elements<stack.size() && stack[sorted_elements] <= stack[sorted_elements-1] )
                sorted_elements++;
            

            return sorted_elements;
        }

        static int gvalue(const deque<int>& stack){
            if (stack.size()==0) return 100;
            else return stack.back();
        }

        void print() const{
            for(auto s : stacks){
                cout << "[" ;
                for(auto e : s) cout << e << " ";
                cout << "]\n";
            }
        }

        void print(deque <deque <int>>& stacks) const{
            for(auto s : stacks){
                cout << "[" ;
                for(auto e : s) cout << e << " ";
                cout << "]\n";
            }
        }

        //functions for computing lb
        void cummulative_demand(map <int, int, std::greater<int> >& demand);
        void availability(map <int, int, std::greater<int>>& available);
        pair<int,int> gv_max_lack(map <int, int, std::greater<int>>& demand, map <int, int, std::greater<int>>& available);
        
        int lb2();

        //retorna la minima cantidad de elementos mal ubicados en un stack (sin contar los stack llenos y ordenados?)
        int min_bad_located();

        //calcula el costo para cada stack asociado al item con group value=gv
        void compute_costs(int gv, vector<int>& costs);

        int min_nx()
        {
            int l =stacks.size();
            float suma=0;
            vector <int> valores;
            for (int i = 0;i<l;i++)
            {
                suma = 0;
                //Saco el largo del stack
                int stack_size = stacks[i].size();
                //Lo paso al vector
                deque<int> v = stacks[i];

                int flag = 0;
                for (int j = 0;j<stack_size-1;j++)
                {
                    if (flag == 1 )
                    {
                        suma += 1;
                    }
                    else
                    {
                         if (v[j]<v[j+1])
                        {
                            suma += 1;
                            flag = 1;
                        }
                    }
                }
                //cout << suma << "\n";
                valores.push_back(suma);
            }

            int min_value = *std::min_element(valores.begin(),valores.end());
            return min_value;
        }

};
}


#endif