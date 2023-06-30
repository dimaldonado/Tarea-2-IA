//
//  tutorial.cpp
//  RLTutorial
//
//  Created by Julio Godoy on 11/25/18.
//  Copyright © 2018 Julio Godoy. All rights reserved.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <math.h>
#include <cmath>
#include <cstdlib>
#include <time.h>
#include <string.h>
#include <fstream>

using namespace std;

int height_grid, width_grid, action_taken, action_taken2,current_episode;
int maxA[100][100], blocked[100][100];
float maxQ[100][100], cum_reward,Qvalues[100][100][4], reward[100][100],finalrw[50000];
int init_x_pos, init_y_pos, goalx, goaly, x_pos,y_pos, prev_x_pos, prev_y_pos, blockedx, blockedy,i,j,k;
ofstream reward_output;

//////////////
//Setting value for learning parameters
int action_sel=2; // 1 is greedy, 2 is e-greedy
int environment= 2; // 1 is small grid, 2 is Cliff walking
int algorithm = 1; //1 is Q-learning, 2 is Sarsa
int stochastic_actions=0; // 0 is deterministic actions, 1 for stochastic actions
int num_episodes=250; //total learning episodes
float learn_rate=0.1; // how much the agent weights each new sample
float disc_factor=0.99; // how much the agent weights future rewards
float exp_rate=0.3; // how much the agent explores
float epsilon = 0.005;
///////////////

void update_q_prev_state() //Updates the Q value of the previous state
{
    
    //Determine the max_a(Qvalue[x_pos][y_pos])
    
    //Update the Q value of the previous state and action if the agent has not reached a terminal state
    if(!( ((x_pos==goalx)&&(y_pos==goaly)) ||((environment==1)&&(x_pos==goalx)&&(y_pos==(goaly-1)))||((environment==2)&&(x_pos>0)&&(x_pos<goalx)&&(y_pos==0))) )
    {
        
        
        Qvalues[prev_x_pos][prev_y_pos][action_taken]= (1-learn_rate)*Qvalues[prev_x_pos][prev_y_pos][action_taken] + (learn_rate*(reward[x_pos][y_pos] + (disc_factor*Qvalues[x_pos][y_pos][action_taken])));
         //How should the Q values be updated?

    }
    else//Update the Q value of the previous state and action if the agent has reached a terminal state
    {
        Qvalues[prev_x_pos][prev_y_pos][action_taken]=  (1-learn_rate)*Qvalues[prev_x_pos][prev_y_pos][action_taken] + (learn_rate*(reward[x_pos][y_pos] + (disc_factor*Qvalues[x_pos][y_pos][action_taken])));

    }
}

void Initialize_environment()
{
    if(environment==1)
    {
        
        height_grid= 3;
        width_grid=4;
        goalx=3;
        goaly=2;
        init_x_pos=0;
        init_y_pos=0;

    }
    
    
    if(environment==2)
    {
    
        height_grid= 4;
        width_grid=12;
        goalx=11;
        goaly=0;
        init_x_pos=0;
        init_y_pos=0;

    }
    
    
    
    
    
    
    for(i=0; i < width_grid; i++)
    {
        for(j=0; j< height_grid; j++)
        {
            
            
            if(environment==1)
            {
                reward[i][j]=-0.04; //-1 if environment 2
                blocked[i][j]=0;
                
            }
            
            
            if(environment==2)
            {
                reward[i][j]=-1;
                blocked[i][j]=0;
            }
            
            
            for(k=0; k<4; k++)
            {
                Qvalues[i][j][k]=rand()%10;
                cout << "Initial Q value of cell [" <<i << ", " <<j << "] action " << k << " = " << Qvalues[i][j][k] << "\n";
            }
            
        }
        
    }
    
    if(environment==1)
    {
        reward[goalx][goaly]=100;
        reward[goalx][(goaly-1)]=-100;
        blocked[1][1]=1;
    }
    
    if(environment==2)
    {
        reward[goalx][goaly]=1;
        
        for(int h=1; h<goalx;h++)
        {   
            reward[h][0]=-100;
            
        }
        
    }
    
}



int action_selection()
{ // Based on the action selection method chosen, it selects an action to execute next
    int best_action = 0;
    float max_value = Qvalues[x_pos][y_pos][0];

    
    if(action_sel==1) //Greedy, always selects the action with the largest Q value
    {
        for(int i= 0;i<4;i++){
            if(max_value< Qvalues[x_pos][y_pos][i]){
                max_value = Qvalues[x_pos][y_pos][i];
                best_action = i;    
            }
            cout<<"accion "<< i <<" valor: "<<Qvalues[x_pos][y_pos][i]<<endl;
        }
        cout<<"mejor accion: "<<best_action<<endl;
        action_taken = best_action;   
        return best_action; //Currently returing a random action, need to code the greedy strategy
    }
    
    if(action_sel==2)//epsilon-greedy, selects the action with the largest Q value with prob (1-exp_rate) and a random action with prob (exp_rate)
    {

        float random_num = static_cast<float>(rand()) / RAND_MAX;
        
        if(random_num>epsilon){
            for(int i= 0;i<4;i++){
                if(max_value< Qvalues[x_pos][y_pos][i]){
                    max_value = Qvalues[x_pos][y_pos][i];
                    best_action = i;    
                }
            }
            action_taken = best_action;   
            return best_action;
            //calculos (selecciona la mejor accion etc)
        }
        else{
            action_taken = rand()%4;
            return action_taken; //Currently returing a random action, need to code the e-greedy strategy 
        
        }
    }
    return 0;
}

void move(int action)
{
    prev_x_pos=x_pos; //Backup of the current position, which will become past position after this method
    prev_y_pos=y_pos;
    bool right = false;
    bool left = false;
    //Stochastic transition model (not known by the agent)
    //Assuming a .8 prob that the action will perform as intended, 0.1 prob. of moving instead to the right, 0.1 prob of moving instead to the left
    

    if(stochastic_actions)
    {
        float random_num = static_cast<float>(rand()) / RAND_MAX;

        if(random_num <= 0.8){
            right = false;
            left = false;
        }
        else if(random_num > 0.8 && random_num <= 0.9){
            right = true;
        }
        else if(random_num>= 0.9){
            left = true;

        }

        //Code here should change the value of variable action, based on the stochasticity of the action outcome
    }
    
    //After determining the real outcome of the chosen action, move the agent
    
    if(action==0) // Up
    {
        
        if((y_pos<(height_grid-1))&&(blocked[x_pos][y_pos+1]==0)) //If there is no wall or obstacle Up from the agent
        {
            y_pos=y_pos+1;  //move up
        }
        
    }
    
    if(action==1)  //Right
    {
        
        if((x_pos<(width_grid-1))&&(blocked[x_pos+1][y_pos]==0)) //If there is no wall or obstacle Right from the agent
        {
            x_pos=x_pos+1; //Move right
        }
        
    }
    
    if(action==2)  //Down
    {
        
        if((y_pos>0)&&(blocked[x_pos][y_pos-1]==0)) //If there is no wall or obstacle Down from the agent
        {
            y_pos=y_pos-1; // Move Down
        }
        
    }
    
    if(action==3)  //Left
    {
        
        if((x_pos>0)&&(blocked[x_pos-1][y_pos]==0)) //If there is no wall or obstacle Left from the agent
        {
            x_pos=x_pos-1;//Move Left
        }
        
    }
    if(stochastic_actions){ 
        if(right){
            if((x_pos<(width_grid-1))&&(blocked[x_pos+1][y_pos]==0)) //If there is no wall or obstacle Right from the agent
            {
                x_pos=x_pos+1; //Move right
            }
            right = false;
        }
        if(left){
            if((x_pos>0)&&(blocked[x_pos-1][y_pos]==0)) //If there is no wall or obstacle Left from the agent
            {
                x_pos=x_pos-1;//Move Left
            }
            left = false;
        }
    }
    update_q_prev_state();
  }



void update_q_prev_state_sarsa()
{
    //Update the Q value of the previous state and action if the agent has not reached a terminal state
    if(!( ((x_pos==goalx)&&(y_pos==goaly)) ||((environment==1)&&(x_pos==goalx)&&(y_pos==(goaly-1)))||((environment==2)&&(x_pos>0)&&(x_pos<goalx)&&(y_pos==0))     ) )
    {
        
       Qvalues[prev_x_pos][prev_y_pos][action_taken]= Qvalues[prev_x_pos][prev_y_pos][action_taken];
    }
    else//Update the Q value of the previous state and action if the agent has reached a terminal state
    {
       Qvalues[prev_x_pos][prev_y_pos][action_taken]= Qvalues[prev_x_pos][prev_y_pos][action_taken];
    }
    
    
}

void Qlearning()
{
   //Follow the  steps in the pseudocode in the slides
   move(action_selection());
   cum_reward=cum_reward+reward[x_pos][y_pos]; //Add the reward obtained by the agent to the cummulative reward of the agent in the current episode
}

void Sarsa()
{
    move(action_selection());
    cum_reward=cum_reward+reward[x_pos][y_pos]; //Add the reward obtained by the agent to the cummulative reward of the agent in the current episode
    
    
}

void Multi_print_grid()
{
    int x, y;
    
    for(y = (height_grid-1); y >=0 ; --y)
    {
        for (x = 0; x < width_grid; ++x)
        {

            if(blocked[x][y]==1) {
                cout << " \033[42m# \033[0m";
               
            }else{
                if ((x_pos==x)&&(y_pos==y)){
                    cout << " \033[44m1 \033[0m";
                    
                }else{
                    cout << " \033[31m0 \033[0m";
                        
                    
                }
            }
        }
        printf("\n");
    }
    printf("\n");
}

void guardarEpisodio(int numeroEpisodio, int recompensa) {
    std::ofstream archivo("Episodios.txt", std::ios_base::app);
    if (archivo.is_open()) {
        archivo << numeroEpisodio << "," << recompensa << "\n";
        archivo.close();
    } else {
        std::cout << "No se pudo abrir el archivo para guardar los episodios." << std::endl;
    }

}
int main(int argc, char* argv[])
{
    srand(time(NULL));
    

    Initialize_environment();//Initialize the features of the chosen environment (goal and initial position, obstacles, rewards)

    for(i=0;i<num_episodes;i++)
    {
        cout << "\n \n Episode " << i<<"----------------------------------------------\n";
        current_episode=i;
        x_pos=init_x_pos;
        y_pos=init_y_pos;
        cum_reward=0;

       
        
        //If Sarsa was chosen as the algorithm:
        if(algorithm==2)
        {
         action_taken= action_selection();
            
        }
        
        //While the agent has not reached a terminal state:
        while(!( ((x_pos==goalx)&&(y_pos==goaly)) ||((environment==1)&&(x_pos==goalx)&&(y_pos==(goaly-1)))||((environment==2)&&(x_pos>0)&&(x_pos<goalx)&&(y_pos==0))     ) )
        {
            if(algorithm==1)
            {
            Multi_print_grid();
            cout<<"estado actual: "<<x_pos<<","<<y_pos<<"  reward: "<<reward[x_pos][y_pos]<<endl; 
            Qlearning();
            }
            if(algorithm==2)
            {
            Sarsa();
            }
            
        }
        guardarEpisodio(i,cum_reward);
        finalrw[i]=cum_reward;
        
        cout << " Total reward obtained: " <<finalrw[i] <<"\n";
        
       
        
    }

    return 0;
}

