#include "Layout.h"

using namespace std;

namespace cpmp {

    int Layout::min_bad_located(){
        int menor=8000000;
        
        for (int hh=0;hh<stacks.size();hh++)
        {
            if (H==sorted_elements[hh]) continue;

            int hhh =stacks[hh].size() - sorted_elements[hh];
            if (menor > hhh) menor = hhh;
        }

        return menor;
    }

    void Layout::compute_costs(int gv, vector<int>& costs)
        {
            
            for (int i = 0;i<stacks.size();i++)
            {
                int cost = 0;

                for(int k=sorted_elements[i]-1; k>=0;k--){
                    if (stacks[i][k]<gv) cost++;
                    else break;
                }

                costs[i]=cost;      
            }
        }


    void Layout::cummulative_demand(map <int, int, std::greater<int> >& demand){
        //demand for each gv
        for (int i=0;i<stacks.size();i++)
        {
            //unsorted items
            for(int j=sorted_elements[i]; j<stacks[i].size();j++){
                int c = stacks[i][j];
                if (demand.find(c)!=demand.end()) demand[c]+=1;
                else demand[c]=1;
            }
        }

        //cumulative demand
        int acum = 0;
        for(pair<int,int> d : demand){
            acum+=d.second;
            demand[d.first]=acum;
        }
    }

    void Layout::availability(map <int, int, std::greater<int>>& available){
        for (int i = 0;i<stacks.size();i++){
            if (sorted_elements[i]==0){
                int c=*gvalues.begin();
                if (available.find(c)!=available.end()) available[c] += H;
                else available[c] = H;
            }

            for(int j=0; j<sorted_elements[i]; j++){
                int c = stacks[i][j];
                if (available.find(c)!=available.end()) available[c]+= H-j-1;
                else available[stacks[i][j]] =  H-j;
            }
        }
    }

    pair<int,int> Layout::gv_max_lack(map <int, int, std::greater<int>>& demand, map <int, int, std::greater<int>>& available){
        int acum = 0;
        int Emax=0,gvv=-1;
        for(int gv : gvalues){
            if(available.find(gv)!=available.end()) acum+=available[gv];
            if(demand.find(gv)!=demand.end()){
                int E=demand[gv]-acum; //carencia(gv)
                if (E>Emax) {
                    Emax=E;
                    gvv = gv;
                }
            }   
        }
        return make_pair(gvv,Emax);
    }

    



    int Layout::lb2(){
        int bx = unsorted_elements;
        int Nbx = bx+min_nx(), Ngx=0;

        //faster and worse version
        //this->lb = Nbx+Ngx + steps;
        //return this->lb;


        
        map <int, int, std::greater<int> > demand;
        cummulative_demand(demand);
        
        map <int, int, std::greater<int>> available; //gv -> D
        availability(available);
        

        //maxima carencia
        pair<int,int> p=gv_max_lack(demand, available);
        int gvv=p.first;
        int Emax=p.second;
        //cout << "Emax:" << Emax << ", " << "gv:" << gvv <<  endl;

        //stacks que se deben desmantelar
        int n = ceil((double)Emax/(double)H-0.0001);

        
        //ordenar stacks menor a mayor cantidad de items ordenados con gv<gvv
        //recorrer los primeros n
        if(n>0){
            list<int> nrs;
            for (int i = 0;i<stacks.size();i++){
                //conisdered by availability
                if(sorted_elements[i] == 0 || stacks[i][sorted_elements[i]-1]>=gvv) continue; 

                int nr=1; //at least one item should be removed
                for(int j=sorted_elements[i]-2; j>=0; j--){
                    if (stacks[i][j] < gvv) nr++;
                    else break;
                }

                nrs.push_back(nr);
            }

            nrs.sort();
            for(int nr:nrs){
                n--;
                Ngx+=nr;
                if(n==0) break;
            }

        }

        //cout << Nbx << "+" << Ngx << "+" << steps << endl;
        this->lb = Nbx + Ngx + steps;
        return this->lb;

    }    
}