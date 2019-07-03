#include<bits/stdc++.h>
using namespace std;

int bag = 150; // the number of point to train a decision tree
int forest = 30;	// the number of the decision tree to vote
int datasize = 200;	// how many point

struct node{
	float x, y;
	int category;
};	// point's x, y and class

struct Tree{
	vector<node > data;
	char direction;
	float position;
	int final;
};	// decision tree's node to store point and attribute

bool comparex(node a, node b){
		if(a.x < b.x){
			return true;
		}else if(a.x == b.x){
			return (a.y < b.y);
		}else{
			return false;
		}
}	// to sort x

bool comparey(node a, node b){
		if(a.y < b.y){
			return true;
		}else if(a.y == b.y){
			return (a.x < b.x);
		}else{
			return false;
		}
}	// to sort y

float findgini(vector<node > &data, char &direction){	// for decision tree's node to decide 
																											//which attribute to choose
	int total = data.size();
	float min = 100;
	float seperate;	// the threshold

	sort(data.begin(),data.end(), comparex);
	for(int i=0; i<total-1; i++){
		float bipart = (data[i].x + data[i+1].x) / 2;
		float left = 0;
		float left1 = 0;
		float left2 = 0;
		float right = 0;
		float right1 = 0;
		float right2 = 0;
		for(int j=0; j<total; j++){
			if(data[j].x >= bipart) {
				right++;
				if(data[j].category == 1) right1++;
				else right2++;
			}else{ 
				left++;
				if(data[j].category == 2) left1++;
				else left2++;
			}
		}
		float ginileft = 1 - (left1/left)*(left1/left) - (left2/left)*(left2/left);
		float giniright = 1 - (right1/right)*(right1/right) - (right2/right)*(right2/right);
		float gini = ginileft*(left/total) + giniright*(right/total);
		if(gini < min){
			min = gini;
			seperate = bipart;
			direction = 'x';
		}
	}	// search threshold of x by gini

	sort(data.begin(),data.end(), comparey);
	for(int i=0; i<total-1; i++){
		float bipart = (data[i].y + data[i+1].y) / 2;
		float left = 0;
		float left1 = 0;
		float left2 = 0;
		float right = 0;
		float right1 = 0;
		float right2 = 0;
		for(int j=0; j<total; j++){
			if(data[j].y >= bipart) {
				right++;
				if(data[j].category == 1) right1++;
				else right2++;
			}else{ 
				left++;
				if(data[j].category == 2) left1++;
				else left2++;
			}
		}
		float ginileft = 1 - (left1/left)*(left1/left) - (left2/left)*(left2/left);
		float giniright = 1 - (right1/right)*(right1/right) - (right2/right)*(right2/right);
		float gini = ginileft*(left/total) + giniright*(right/total);
		if(gini < min){
			min = gini;
			seperate = bipart;
			direction = 'y';
		}
	}	// search threshold of y by gini, choose the smallist one compare to x

	return seperate; 
}

void buildtree(vector<Tree > &DT){	// build decision tree by vector
	for(int i=0; i<bag; i++){
		if(DT[i].data.size() == 0){	// no data == no such child root
			continue;
		}else{
			bool same = true;
			for(int j=0; j<DT[i].data.size()-1; j++){
				if(DT[i].data[j].category != DT[i].data[j+1].category) same = false;
			}
			if(same){	// doesn't need to seperate
				DT[i].final = DT[i].data[0].category; 
			}else{
				char direction;
				float seperate;
				seperate = findgini(DT[i].data, direction);
				if(direction == 'x'){
					for(int j=0; j<DT[i].data.size(); j++){
						if(DT[i].data[j].x >= seperate){
							DT[2*i+2].data.push_back(DT[i].data[j]);
						}else{
							DT[2*i+1].data.push_back(DT[i].data[j]);
						}
					}
				}else{
					for(int j=0; j<DT[i].data.size(); j++){
						if(DT[i].data[j].y >= seperate){
							DT[2*i+2].data.push_back(DT[i].data[j]);
						}else{ 
							DT[2*i+1].data.push_back(DT[i].data[j]);
						}
					}
				}
				DT[i].direction = direction;
				DT[i].position = seperate;
			}	// use gini to seperate and memorize the threshold
		}
	}
}	

float depend(node &test, vector<vector<Tree > > &DT){	// use to vote a point is in class 1 or 2
	int vote1 = 0;
	int vote2 = 0;
	for(int i=0; i<forest; i++){
		for(int j=0; j<50 ;){
			if(DT[i][j].final == 1){ // semms to be class 1
				vote1++;
				break;
			}else if(DT[i][j].final == 2){	//seems to be class 2
				vote2++;
				break;
			}else if(DT[i][j].direction == 'x'){	
				if(test.x >= DT[i][j].position){	// right child
					j = j*2+2;
				}else{	// left child
					j = j*2+1;
				}
			}else if(DT[i][j].direction == 'y'){
				if(test.y >= DT[i][j].position){	// right child 
					j = j*2+2;
				}else{	// left child
					j = j*2+1;
				}
			}else{	// can't decide (actually i think it's a kind of bug, but i have no idea about it)
				break;
			}
		}
	}
	if(vote1 >= vote2) return 1;
	else return 2;
}

int main(){
	srand( time(NULL));	//	set random
	//ifstream fin("ellipse100.txt", ios::in);	// read data
	ifstream fin("cross200.txt", ios::in);	// read data
	vector<node > nodelist;	// store point 
	string str;
	while(getline(fin, str)){
		istringstream iss(str);
		node temp;
		iss >> temp.x;
		iss >> temp.y;
		iss >> temp.category;
		nodelist.push_back(temp);
	}		// read data

	vector<vector<Tree > > DT(forest, vector<Tree>(1000)); // decision tree forest

	for(int i=0; i<forest; i++){
		int random[datasize];
		for(int j=0; j<datasize; j++){
			random[j] = j; 
		}
		for(int j=0; j<datasize; j++){
			int pos = rand() % datasize;
			int temp;
			temp = random[j];
			random[j] = random[pos];
			random[pos] = temp;
		} // get random number set

		
		for(int j=0; j<bag; j++){
			DT[i][0].data.push_back(nodelist[random[j]]);
		}	// random point at root for each decision tree
				
		buildtree(DT[i]);	// build it!!
	}

	float fitrate;
	float fitnode = 0;
	for(int i=0; i<datasize; i++){
		if(nodelist[i].category == depend(nodelist[i], DT)) fitnode++;
	}	// test each node to see if it fit the result of vote
	fitrate = fitnode / datasize;	// compute error rate
	cout << "bag :" << bag << endl;
	cout << "forest: " << forest << endl;
	cout << fitrate << endl;

	return 0;
}