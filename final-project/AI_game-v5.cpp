#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <cfloat>
#include <algorithm>
#include <stdio.h>
using namespace std;

/*adjustable parameter*/
/*The constant of the UCB function*/
#define UCB_const 1.414
/*The parameter of the heuristic function*/
#define A 13
#define B 3

/*global variable*/
int all_sample_time = 0;//the all times of doing sample in MCTS
int self;//determine the color of our piece
int opponent;//determine the color of opponent's piece
vector<vector<int> > local_board(8, vector<int>(8,0));

/*The node of the tree*/
struct tree_node{
	float time;//the times of this node be sampled
	float win_time;//the win times of this node
	int depth;//this is using to check the node's position for debug 
	bool is_sampled;//setting is_sampled to true when the node has been sampled
	vector<vector<int> > step;//the step of moving the piece to this node's board state   
	vector<tree_node *> children;//store the pointer to childen
};

/*function define(implement below)*/

/*utility function*/
float UCB(tree_node *x);
/*change the state of local_board according to the input step */
void move_a_piece(vector<vector<int> > &next_step);
/*give it a piece's position and get its movable step*/
std::vector<std::vector<int> > GetCanMove(std::vector<std::vector<int> >& board, bool is_black, int nowx, int nowy);
/*recursively to find the ExpandCildMove*/
vector<int> findPre(vector<vector<int> >& map, int nowx, int nowy, int nowwant);
/*return the next step*/
vector<vector<vector<int> > > ExpandChildMove(std::vector<std::vector<int> >& board, bool is_black, int nowx, int nowy);
/*sample : simulate the game*/
int simulation(int round);
/*print the state of local board*/
void print_board();
/*The data structure of the tree*/
class tree{
	public :
		/*add a child to any node on tree*/
		/*parameter depth & num are temporary using for debug*/
		void add_child(tree_node *x,int depth , vector<vector<int> > next_step){
			tree_node *temp_ptr = new tree_node;
			temp_ptr->time = 0;
			temp_ptr->win_time = 0;
			temp_ptr->depth = depth;
			temp_ptr->is_sampled = false;
			temp_ptr->step = next_step;
			x->children.push_back(temp_ptr);
		}
		/*print the data of the node x*/
		void print_node(tree_node *x){
			cout<<"Sample times : "<<x->time<<"\n";
			cout<<"Win times : "<<x->win_time<<"\n";
			cout<<"Depth : "<<x->depth<<"\n";
			cout<<"Children num : "<<x->children.size()<<"\n";
			cout<<"Utility : "<<UCB(x)<<"\n";
			if(x->is_sampled)
				cout<<"is_sample : true"<<"\n\n";
			else
				cout<<"is_sample : false"<<"\n\n";
		}
		/*tree's traversal*/
		void traversal(){
			pre_order_traversal(root);
		}
		void pre_order_traversal(tree_node *current_node){
			print_node(current_node);
//			time_t start_t,current_t;
//			time(&start_t);
//			do{
//				time(&current_t);
//			}while((current_t - start_t) < 1);
			for(int i = 0 ; i < current_node->children.size() ; i++)
				pre_order_traversal(current_node->children[i]);
		}
		/*sample the node and return the result*/
		float sample_node(tree_node *target_node){
			return (float)simulation(target_node->depth);
		}
		/*expand the node which is sampled and is selected again*/
		/*piece_color is using to determine whose piece (self or opponent) is moved */
		void expand_node(tree_node *target_node,int piece_color){
			vector<vector<vector<int> > > children_step;// all the step this node can move 
			vector<vector<vector<int> > >::iterator it;
			for(int i = 0 ; i < 8 ; i++)
				for(int j = 0 ; j < 8 ; j++){
					if(local_board[i][j] == piece_color)
						children_step = ExpandChildMove(local_board,true,i,j);
					if(children_step.size() != 0){
						for(it = children_step.begin() ; it != children_step.end() ; it++)
							add_child(target_node,target_node->depth+1,*it);
						children_step.clear();
					}
				}
			
		}
		/*select a node to be sampled in Monte-Carlo tree*/
		void MCTS_select(){
			float result;
			/*selection begins from the root*/
			if(root->is_sampled){
				/*root is sampled treverse its child*/
//				printf("root : \n\n");
//				print_board();
				result = select_max_node(root);//select_max_node implement in private
			}
			else{
				/*root isn't sampled so sample it*/
				result = sample_node(root);
				all_sample_time++;
				root->win_time += result;
				root->time++;
				root->is_sampled = true; 
			}
		}
		/*using local board to store the previous board state*/
		/*move the root according to opponent's move*/
		void move_root(vector<vector<int> > &current_board){
			int start_point[2],end_point[2];
			bool start = false,end = false,find = false;
			for(int i = 0 ; i < 8 ; i++ ){
				for(int j = 0 ; j < 8 ; j++){
					if(end && start)
						break;
					if(!end && local_board[i][j] == 0 && current_board[i][j] == opponent){
						end_point[0] = i;
						end_point[1] = j;
						end = true;
					}
					else if(!start && local_board[i][j] == opponent && current_board[i][j] == 0){
						start_point[0] = i;
						start_point[1] = j;
						start = true;
					}
				}
				if(end && start)
					break;
			}
			for(int i = 0 ; i < root->children.size() ; i++){
				if(root->children[i]->step[0][0] == start_point[0] && root->children[i]->step[0][1] == start_point[1])
					if(root->children[i]->step[root->children[i]->step.size()-1][0] == end_point[0] && root->children[i]->step[root->children[i]->step.size()-1][1] == end_point[1]){
						root = root->children[i];
						find = true;
						break;
					}
			}
			if(!find){
				root = new tree_node;
				root->time = 0;
				root->win_time = 0;
				root->depth = root->depth+1;
				root->is_sampled = false;
			}
		}
		/*return the best step computed by utility function and move the root to this child node*/
		vector<vector<int> > return_step(){
			float max_utility = -1,child_utility;
			int best_child_index;
			for(int i = 0 ; i < root->children.size() ; i++){
				child_utility = UCB(root->children[i]);
				if(child_utility > max_utility){
					max_utility = child_utility;
					best_child_index = i;
				}
			}
			root = root->children[best_child_index];
			move_a_piece(root->step);
			return root->step;
		}
		/*get the pointer of root*/
		tree_node *get_root(){
			return root;
		}
		/*tree constructor*/
		tree(){
			root = new tree_node;
			root->time = 0;
			root->win_time = 0;
			root->depth = 0;
			root->is_sampled = false;
		}
	private : 
		/*Tree's root*/
		tree_node *root;
		/*select a node from those node having same utility*/
		int random_select_node(vector<int> &nodes){
			return nodes[rand()%nodes.size()];
		}
		/*traversal the tree to select an unsampled node with max utility using utility function to be sampled*/
		float select_max_node(tree_node *current_node){
			/*current node isn't sampled*/
			/*sample it and then update the node on path*/
			if(!current_node->is_sampled){
				float result = sample_node(current_node);//sample node and get the result
				all_sample_time++;
				/*update current node*/
				current_node->win_time += result;
				current_node->time++;
				current_node->is_sampled = true;
				/*return the result to update the node on the treversal's path*/
				return result;
			}
			/*current node is sampled*/
			/*if current node is leaf node expand it and randomly select a child node to sample*/
			if(current_node->children.size()==0){
				expand_node(current_node,self);
			}
			/*select the "best" node using utility function*/
			float max_utility = -1,child_node_utility;
			int best_child_index;
			vector<int> candidate_child_node;//the nodes have the same good uility
			for(int i = 0 ; i < current_node->children.size() ; i++){
				child_node_utility = UCB(current_node->children[i]);
				if(child_node_utility > max_utility){
					/*finding the better child clear the previos candidate*/
					candidate_child_node.clear();
					max_utility = child_node_utility;
					candidate_child_node.push_back(i);
				}
				else if(child_node_utility == max_utility){
					candidate_child_node.push_back(i);
				}
			}
			/*randomly select*/
			best_child_index = random_select_node(candidate_child_node);
			tree_node *best_child_node = current_node->children[best_child_index];
			move_a_piece(best_child_node->step);
//			printf("depth : %d\n\n",current_node->depth);
//			print_board();
			float result = select_min_node(best_child_node);//sample child node and get the result
			/*update current node*/
			current_node->win_time += result;
			current_node->time++;
			/*return the result to update the node on the treversal's path*/
			return result;
		}
		/*traversal the tree to select an unsampled node with min utility using utility function to be sampled*/
		float select_min_node(tree_node *current_node){
			/*current node isn't sampled*/
			/*sample it and then update the node on path*/
			if(!current_node->is_sampled){
				float result = sample_node(current_node);//sample node and get the result
				all_sample_time++;
				/*update current node*/
				current_node->win_time += result;
				current_node->time++;
				current_node->is_sampled = true;
				/*return the result to update the node on the treversal's path*/
				return result;
			}
			/*current node is sampled*/
			/*if current node is leaf node expand it and randomly select a child node to sample*/
			if(current_node->children.size()==0){
				expand_node(current_node,opponent);
			}
			/*select the "best" node using utility function*/
			float min_utility = 10,child_node_utility;
			int best_child_index;
			vector<int> candidate_child_node;//the nodes have the same good uility
			for(int i = 0 ; i < current_node->children.size() ; i++){
				if(current_node->children[i]->is_sampled)
					child_node_utility = UCB(current_node->children[i]);
				else
					child_node_utility = -1;//give the unsampled node the min utility
				if(child_node_utility < min_utility){
					/*finding the better child clear the previos candidate*/
					candidate_child_node.clear();
					min_utility = child_node_utility;
					candidate_child_node.push_back(i);
				}
				else if(child_node_utility == min_utility){
					candidate_child_node.push_back(i);
				}
			}
			/*randomly select*/
			best_child_index = random_select_node(candidate_child_node);
			tree_node *best_child_node = current_node->children[best_child_index];
			move_a_piece(best_child_node->step);
//			printf("depth : %d\n\n",current_node->depth);
//			print_board();
			float result = select_max_node(best_child_node);//sample child node and get the result
			/*update current node*/
			current_node->win_time += result;
			current_node->time++;
			/*return the result to update the node on the treversal's path*/
			return result;
		}
};

/*The main object of the Monte-Carlo tree*/
tree MCTS;

/*For the debug associating with board*/
void add_one_step(vector<vector<int> > &step,int row,int col){
	vector<int> one_step;
	one_step.push_back(row);
	one_step.push_back(col);
	step.push_back(one_step);
}
void print_board(){
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			printf("%d",local_board[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void print_selected_board(vector<vector<int > > &map){
	int one_piece = 0 , two_piece = 0;
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			if(map[i][j] == 1)
			    one_piece++;
			if(map[i][j] == 2)
			    two_piece++;
			printf("%d",map[i][j]);
		}
		printf("\n");
	}
	printf("\n");
	cout<<"1 piece num : "<<one_piece<<"\n2 piece num : "<<two_piece<<"\n\n";
}

void initial_board(){
	for(int i = 0 ; i < 8 ; i++)
		for(int j = 0 ; j < 8 ; j++)
			local_board[i][j]=0;
	for(int i = 0 ; i < 4 ;i++){
		if(i<2){
			for(int j = 0 ; j < 3 ; j++)
				local_board[i*2+j][j] = 1;
		}
		else if(i==2){
			for(int j = 0 ; j < 2 ; j++)
				local_board[i*2+j][j] = 1;
		}
		else
			local_board[i*2][0] = 1;
	}
	for(int i = 0 ; i < 4 ;i++){
		if(i<2){
			for(int j = 0 ; j < 3 ; j++)
				local_board[1+i*2+j][7-j] = 2;
		}
		else if(i==2){
			for(int j = 0 ; j < 2 ; j++)
				local_board[1+i*2+j][7-j] = 2;
		}
		else
			local_board[1+i*2][7] = 2;
	}
}

void initial_select_board(vector<vector<int > > &map){
	for(int i = 0 ; i < 8 ; i++)
		for(int j = 0 ; j < 8 ; j++)
			map[i][j]=0;
	for(int i = 0 ; i < 4 ;i++){
		if(i<2){
			for(int j = 0 ; j < 3 ; j++)
				map[i*2+j][j] = 1;
		}
		else if(i==2){
			for(int j = 0 ; j < 2 ; j++)
				map[i*2+j][j] = 1;
		}
		else
			map[i*2][0] = 1;
	}
	for(int i = 0 ; i < 4 ;i++){
		if(i<2){
			for(int j = 0 ; j < 3 ; j++)
				map[1+i*2+j][7-j] = 2;
		}
		else if(i==2){
			for(int j = 0 ; j < 2 ; j++)
				map[1+i*2+j][7-j] = 2;
		}
		else
			map[1+i*2][7] = 2;
	}
}

/*function implement*/
float UCB(tree_node *x){
	if(x->time==0)
		return FLT_MAX;
	else
		return(x->win_time/x->time+UCB_const*sqrt(log10(all_sample_time)/x->time));
}

void move_a_piece(vector<vector<int> > &next_step){
	if(next_step.size() <= 1 )
		return;
	/*next position the piece will move to*/
	int next_step_row,next_step_col;
	/*the piece's current position*/
	int current_step_row,current_step_col;
	/*the iterator of the next_step*/
	vector<vector<int> >::iterator it_step;
	/*find the target piece on local_board according to the first step and remove it*/
	int piece_color = local_board[next_step[0][0]][next_step[0][1]];
	local_board[next_step[0][0]][next_step[0][1]] = 0;
	/*move the piece*/
	for(it_step = next_step.begin()+1 ; it_step != next_step.end() ; it_step++){
		current_step_row = (*(it_step - 1))[0];
		current_step_col = (*(it_step - 1))[1];
		next_step_row = (*it_step)[0];
		next_step_col = (*it_step)[1];
		/*if piece jump compare the piece's color between two step to the moving piece'scolor*/
		if(abs(current_step_row - next_step_row) > 1 || abs(current_step_col - next_step_col) > 1){
			if(local_board[(current_step_row+next_step_row)/2][(current_step_col+next_step_col)/2] != piece_color)
					local_board[(current_step_row+next_step_row)/2][(current_step_col+next_step_col)/2] = 0;
		}
	}
	/*mark the final position on local_board*/
	local_board[next_step_row][next_step_col] = piece_color;
}
/*input a piece's position and get the usable move */
std::vector<std::vector<int> > GetCanMove(std::vector<std::vector<int> >& board, bool is_black, int nowx, int nowy){ 
	std::vector<std::vector<int> > map(8, std::vector<int>(8, 0));
	std::vector<std::vector<int> > canMove;

	map[nowx][nowy] = 1;
	if(nowx > 0){
		if(board[nowx-1][nowy] == 0) map[nowx-1][nowy] = 2;
	}
	if(nowy > 0){
		if(board[nowx][nowy-1] == 0) map[nowx][nowy-1] = 2;
	}
	if(nowx < 7){
		if(board[nowx+1][nowy] == 0) map[nowx+1][nowy] = 2;
	}
	if(nowy < 7){
		if(board[nowx][nowy+1] == 0) map[nowx][nowy+1] = 2;
	}

	if(nowx > 1){
		if(board[nowx-1][nowy] != 0 && board[nowx-2][nowy] == 0) map[nowx-2][nowy] = 3;
	}
	if(nowy > 1){
		if(board[nowx][nowy-1] != 0 && board[nowx][nowy-2] == 0) map[nowx][nowy-2] = 3;
	}
	if(nowx < 6){
		if(board[nowx+1][nowy] != 0 && board[nowx+2][nowy] == 0) map[nowx+2][nowy] = 3;
	}
	if(nowy < 6){
		if(board[nowx][nowy+1] != 0 && board[nowx][nowy+2] == 0) map[nowx][nowy+2] = 3;
	}

	for(int i=0; i<18; i++){
		bool modify = false;
		for(int j=0; j<8; j++){
			for(int k=0; k<8; k++){
				if(map[j][k] == 3){
					if(j > 1){
						if(board[j-1][k] != 0 && board[j-2][k] == 0 && map[j-2][k] != 3){
							map[j-2][k] = 3;
							modify = true;
						}
					}
					if(k > 1){
						if(board[j][k-1] != 0 && board[j][k-2] == 0 && map[j][k-2] != 3){
							map[j][k-2] = 3;
							modify = true;
						}
					}
					if(j < 6){
						if(board[j+1][k] != 0 && board[j+2][k] == 0 && map[j+2][k] != 3){
							map[j+2][k] = 3;
							modify = true;
						}
					}
					if(k < 6){
						if(board[j][k+1] != 0 && board[j][k+2] == 0 && map[j][k+2] != 3){
							map[j][k+2] = 3;
							modify = true;
						}
					}
				}
			}
		}
		if(!modify)break;
	}

	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			if(map[i][j] > 0){
				vector<int> temp;
				temp.push_back(i);
				temp.push_back(j);
				canMove.push_back(temp);
			}
		}
	}
	return canMove;
}

vector<vector<vector<int> > > ExpandChildMove(std::vector<std::vector<int> >& board, bool is_black, int nowx, int nowy){ 
	std::vector<std::vector<int> > map(8, std::vector<int>(8, 0));
	vector<vector<vector<int> > > ChildMove;
	vector<int> start;
	start.push_back(nowx);
	start.push_back(nowy);
	vector<vector<int> > stop;
	stop.push_back(start);
	ChildMove.push_back(stop);

	map[nowx][nowy] = 1;
	if(nowx > 0){
		if(board[nowx-1][nowy] == 0){
			map[nowx-1][nowy] = 2;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx-1);
			temp.push_back(nowy);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if(nowy > 0){
		if(board[nowx][nowy-1] == 0) {
			map[nowx][nowy-1] = 2;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx);
			temp.push_back(nowy-1);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if(nowx < 7){
		if(board[nowx+1][nowy] == 0) {
			map[nowx+1][nowy] = 2;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx+1);
			temp.push_back(nowy);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if(nowy < 7){
		if(board[nowx][nowy+1] == 0) {
			map[nowx][nowy+1] = 2;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx);
			temp.push_back(nowy+1);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}

	if(nowx > 1){
		if(board[nowx-1][nowy] != 0 && board[nowx-2][nowy] == 0) {
			map[nowx-2][nowy] = 3;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx-2);
			temp.push_back(nowy);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if(nowy > 1){
		if(board[nowx][nowy-1] != 0 && board[nowx][nowy-2] == 0) {
			map[nowx][nowy-2] = 3;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx);
			temp.push_back(nowy-2);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if(nowx < 6){
		if(board[nowx+1][nowy] != 0 && board[nowx+2][nowy] == 0) {
			map[nowx+2][nowy] = 3;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx+2);
			temp.push_back(nowy);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if(nowy < 6){
		if(board[nowx][nowy+1] != 0 && board[nowx][nowy+2] == 0) {
			map[nowx][nowy+2] = 3;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx);
			temp.push_back(nowy+2);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}

	int c = 4;
	for(int i=0; i<18; i++){
		bool modify = false;
		for(int j=0; j<8; j++){
			for(int k=0; k<8; k++){
				if(map[j][k] == c-1){
					if(j > 1){
						if(board[j-1][k] != 0 && board[j-2][k] == 0 && map[j-2][k] == 0){
							map[j-2][k] = c;
							modify = true;
						}
					}
					if(k > 1){
						if(board[j][k-1] != 0 && board[j][k-2] == 0 && map[j][k-2] == 0){
							map[j][k-2] = c;
							modify = true;
						}
					}
					if(j < 6){
						if(board[j+1][k] != 0 && board[j+2][k] == 0 && map[j+2][k] == 0){
							map[j+2][k] = c;
							modify = true;
						}
					}
					if(k < 6){
						if(board[j][k+1] != 0 && board[j][k+2] == 0 && map[j][k+2] == 0){
							map[j][k+2] = c;
							modify = true;
						}
					}
				}
			}
		}
		if(!modify)break;
		c++;
	}

	int nowwant = 4;
	while(nowwant < 8){
		for(int i=0; i<8; i++){
			for(int j=0; j<8; j++){
				if(map[i][j] == nowwant){
					int nowstep = nowwant;
					vector<int> temp;
					vector<vector<int> > tempstep;
					temp.push_back(i);
					temp.push_back(j);
					tempstep.push_back(temp);
					while(nowstep > 3){
						int s = tempstep.size();
						temp = findPre(map, tempstep[s-1][0], tempstep[s-1][1], nowstep);
						tempstep.push_back(temp);
						nowstep--;
					}
					tempstep.push_back(start);
					reverse(tempstep.begin(), tempstep.end());
					ChildMove.push_back(tempstep);
				}
			}
		}
		nowwant++;
	}

	return ChildMove;
}
vector<int> findPre(vector<vector<int> >& map, int nowx, int nowy, int nowwant){
	nowwant = nowwant - 1;
	if(nowx > 1){
		if(map[nowx-2][nowy] == nowwant){
			vector<int> temp;
			temp.push_back(nowx-2);
			temp.push_back(nowy);
			return temp;
		}
	}
	if(nowy > 1){
		if(map[nowx][nowy-2] == nowwant){
			vector<int> temp;
			temp.push_back(nowx);
			temp.push_back(nowy-2);
			return temp;
		}
	}
	if(nowx < 6){
		if(map[nowx+2][nowy] == nowwant){
			vector<int> temp;
			temp.push_back(nowx+2);
			temp.push_back(nowy);
			return temp;
		}
	}
	if(nowy < 6){
		if(map[nowx][nowy+2] == nowwant){
			vector<int> temp;
			temp.push_back(nowx);
			temp.push_back(nowy+2);
			return temp;
		}
	}
}

void simulate_move(vector<vector<int> > &next_step, vector<vector<int > > &map){
	if(next_step.size() <= 1 ){
        return;
    }
	/*next position the piece will move to*/
	int next_step_row,next_step_col;
	/*the piece's current position*/
	int current_step_row,current_step_col;
	/*the iterator of the next_step*/
	vector<vector<int> >::iterator it_step;
	/*find the target piece on local_board according to the first step and remove it*/
	int piece_color = map[next_step[0][0]][next_step[0][1]];
	map[next_step[0][0]][next_step[0][1]] = 0;
	/*move the piece*/
	for(it_step = next_step.begin()+1 ; it_step != next_step.end() ; it_step++){
		current_step_row = (*(it_step - 1))[0];
		current_step_col = (*(it_step - 1))[1];
		next_step_row = (*it_step)[0];
		next_step_col = (*it_step)[1];
		/*if piece jump compare the piece's color between two step to the moving piece'scolor*/
		if(abs(current_step_row - next_step_row) > 1 || abs(current_step_col - next_step_col) > 1){
			if(map[(current_step_row+next_step_row)/2][(current_step_col+next_step_col)/2] != piece_color)
					map[(current_step_row+next_step_row)/2][(current_step_col+next_step_col)/2] = 0;
		}
	}
	/*mark the final position on local_board*/
	map[next_step_row][next_step_col] = piece_color;
}

int heuristic(int x, int y, const vector<vector<int > > &map) {
	int nowself = map[x][y];
	int ene = nowself == 1 ? 2 : 1;
	int f = 0, g = -1;
	if (nowself == 1) {
		switch (y) {
		case(0):
			g = -3;
			break;
		case(1):
			g = 0;
			break;
		case(2):
			g = 1;
			break;
		case(3):
			g = 2;
			break;
		case(4):
			g = 3;
			break;
		case(5):
			g = 4;
			break;
		case(6):
		case(7):
			g = 5;
			break;
		}
	}
	else {
		switch (y) {
		case(0):
		case(1):
			g = 5;
			break;
		case(2):
			g = 4;
			break;
		case(3):
			g = 3;
			break;
		case(4):
			g = 2;
			break;
		case(5):
			g = 1;
			break;
		case(6):
			g = 0;
			break;
		case(7):
			g = -3;
			break;
		}
	}

	if (y == 7)f++;
	else if (map[x][y + 1] == ene) {
		if (y == 0 || map[x][y - 1] == nowself)f += 2;
		else f -= 4;
	}
	else if (map[x][y + 1] == nowself)f+=2;

	if (y == 0)f++;
	else if (map[x][y - 1] == ene) {
		if (y == 7 || map[x][y + 1] == nowself)f += 2;
		else f -= 4;
	}
	else if (map[x][y - 1] == nowself)f+=2;

	if (x == 7)f -= 3;
	else if (map[x + 1][y] == ene) {
		if (x == 0 || map[x - 1][y] == nowself)f += 2;
		else f -= 4;
	}
	else if (map[x + 1][y] == nowself)f-=1;

	if (x == 0)f -= 3;
	else if (map[x - 1][y] == ene) {
		if (x == 7 || map[x - 1][y] == nowself)f += 2;
		else f -= 4;
	}
	else if (map[x - 1][y] == nowself)f-=1;

	return A * g + B * f + 1000;
}


vector<vector<int > >  simulation_one_step(vector<vector<int > > current_map, int whoseturn){
    int ene = whoseturn == 1 ? 2 : 1;
    vector<vector<int > > finalstep;
    int bestvalue = -99999;
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(current_map[i][j]==whoseturn){
                vector<vector<int > > map = current_map;
                vector<vector<vector<int > > > smove;
                smove = ExpandChildMove(map, whoseturn==1, i, j);

                for(int k=0;k<smove.size();k++){
                    map = current_map;
                    vector<vector<int > > guess_move;
                    guess_move = smove[k];
                    simulate_move(guess_move,map);
                    int selfh=0,eneh=0;
                    int functionvalue=0;
                    for(int m=0;m<8;m++){
                        for(int n=0;n<8;n++){
                            if(map[m][n]==whoseturn) selfh+=heuristic(m, n, map);
                            if(map[m][n]==ene) eneh+=heuristic(m, n, map);
                        }
                    }
                    functionvalue = selfh - eneh;
                    if(functionvalue > bestvalue){
                        bestvalue = functionvalue;
                        finalstep = guess_move;
                    }
                }

            }
        }
    }
    return finalstep;
}

int gameover(vector<vector<int > > &map){
    int bleft=0, breach=0, wleft=0, wreach=0;
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(map[i][j]==1){
                bleft++;
                if(j>5)breach++;
            }
            if(map[i][j]==2){
                wleft++;
                if(j<2)wreach++;
            }
        }
    }
	//cout<<"bleft: "<<bleft<<"\nbreach: "<<breach<<"\nwleft"<<wleft<<"\nwreach"<<wreach<<endl;
    if(bleft==0&&wleft==0) return 3;
    else if((bleft==0 || wleft==0)&&(breach==wreach))return 3;
    else if((bleft==0 || wleft==0)&&(breach>wreach))return 1;
    else if((bleft==0 || wleft==0)&&(breach<wreach))return 2;
	else if((breach==bleft)||(wreach==wleft)){
		if(breach == wreach) return 3;
		return breach > wreach ? 1 : 2;
	}
    else return 0;
}

int simulation(int round){
    int ene = self == 1 ? 2 : 1;
    int whose_turn = ene;
    vector<vector<int > > map = local_board;
    int roundleft = 400-round;
    while(--roundleft){
        vector<vector<int > >  choosestep;
        choosestep = simulation_one_step(map, whose_turn);
        simulate_move(choosestep, map);
        system("pause");
//        for(vector<vector<int> >::iterator it = choosestep.begin() ; it != choosestep.end() ; it++ )
//			printf("(%d,%d) ",(*it)[0],(*it)[1]);
//		cout<<"\n\n";
       print_selected_board(map);
        if(gameover(map)==self) return 1;
        if(gameover(map)!=0) return 0;
        whose_turn = whose_turn == 1 ? 2 : 1;
    }
    return gameover(map) == self ? 1 : 0;
}

int main(){
	/*seed*/
	srand(time(NULL));
   /*set self*/
   self = 1;
   opponent = 2;
   initial_board();
   simulation(200);
	
		  
	return 0;
}
