#include<bits/stdc++.h>
using namespace std;
std::vector<std::vector<int> > GetCanMove(std::vector<std::vector<int> >& board, bool is_black, int nowx, int nowy);
/*recursively to find the ExpandCildMove*/
vector<int> findPre(vector<vector<int> >& map, int nowx, int nowy, int nowwant, vector<vector<int> >& board);
/*return the next step*/
vector<vector<vector<int> > > ExpandChildMove(std::vector<std::vector<int> >& board, bool is_black, int nowx, int nowy);

std::vector<std::vector<int>> GetCanMove(std::vector<std::vector<int>>& board, bool is_black, int nowx, int nowy){ 
	std::vector<std::vector<int>> map(8, std::vector<int>(8, 0));
	std::vector<std::vector<int>> canMove;

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

vector<vector<vector<int> > > ExpandChildMove(std::vector<std::vector<int> >& board, bool is_black, int nowx, int nowy) {
	std::vector<std::vector<int> > map(8, std::vector<int>(8, 0));
	vector<vector<vector<int> > > ChildMove;
	vector<int> start;
	start.push_back(nowx);
	start.push_back(nowy);
//	vector<vector<int> > stop;
//	stop.push_back(start);
//	ChildMove.push_back(stop);

	map[nowx][nowy] = 1;
	if (nowx > 0) {
		if (board[nowx - 1][nowy] == 0) {
			map[nowx - 1][nowy] = 2;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx - 1);
			temp.push_back(nowy);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if (nowy > 0) {
		if (board[nowx][nowy - 1] == 0) {
			map[nowx][nowy - 1] = 2;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx);
			temp.push_back(nowy - 1);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if (nowx < 7) {
		if (board[nowx + 1][nowy] == 0) {
			map[nowx + 1][nowy] = 2;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx + 1);
			temp.push_back(nowy);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if (nowy < 7) {
		if (board[nowx][nowy + 1] == 0) {
			map[nowx][nowy + 1] = 2;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx);
			temp.push_back(nowy + 1);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}

	if (nowx > 1) {
		if (board[nowx - 1][nowy] != 0 && board[nowx - 2][nowy] == 0) {
			map[nowx - 2][nowy] = 3;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx - 2);
			temp.push_back(nowy);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if (nowy > 1) {
		if (board[nowx][nowy - 1] != 0 && board[nowx][nowy - 2] == 0) {
			map[nowx][nowy - 2] = 3;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx);
			temp.push_back(nowy - 2);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if (nowx < 6) {
		if (board[nowx + 1][nowy] != 0 && board[nowx + 2][nowy] == 0) {
			map[nowx + 2][nowy] = 3;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx + 2);
			temp.push_back(nowy);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}
	if (nowy < 6) {
		if (board[nowx][nowy + 1] != 0 && board[nowx][nowy + 2] == 0) {
			map[nowx][nowy + 2] = 3;
			vector<int> temp;
			vector<vector<int> > tempstep;
			temp.push_back(nowx);
			temp.push_back(nowy + 2);
			tempstep.push_back(start);
			tempstep.push_back(temp);
			ChildMove.push_back(tempstep);
		}
	}

	int c = 4;
	for (int i = 0; i < 18; i++) {
		bool modify = false;
		for (int j = 0; j < 8; j++) {
			for (int k = 0; k < 8; k++) {
				if (map[j][k] == c - 1) {
					if (j > 1) {
						if (board[j - 1][k] != 0 && board[j - 2][k] == 0 && map[j - 2][k] == 0) {
							map[j - 2][k] = c;
							modify = true;
						}
					}
					if (k > 1) {
						if (board[j][k - 1] != 0 && board[j][k - 2] == 0 && map[j][k - 2] == 0) {
							map[j][k - 2] = c;
							modify = true;
						}
					}
					if (j < 6) {
						if (board[j + 1][k] != 0 && board[j + 2][k] == 0 && map[j + 2][k] == 0) {
							map[j + 2][k] = c;
							modify = true;
						}
					}
					if (k < 6) {
						if (board[j][k + 1] != 0 && board[j][k + 2] == 0 && map[j][k + 2] == 0) {
							map[j][k + 2] = c;
							modify = true;
						}
					}
				}
			}
		}
		if (!modify)break;
		c++;
	}

	int nowwant = 4;
	while (nowwant < 8) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				if (map[i][j] == nowwant) {
					int nowstep = nowwant;
					vector<int> temp;
					vector<vector<int> > tempstep;
					temp.push_back(i);
					temp.push_back(j);
					tempstep.push_back(temp);
					while (nowstep > 3) {
						int s = tempstep.size();
						temp = findPre(map, tempstep[s - 1][0], tempstep[s - 1][1], nowstep, board);
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

	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			cout << board[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;


	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			cout << map[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	return ChildMove;
}

vector<int> findPre(vector<vector<int> >& map, int nowx, int nowy, int nowwant , vector<vector<int> >& board) {
	nowwant = nowwant - 1;
	if (nowx > 1) {
		if (map[nowx - 2][nowy] == nowwant && board[nowx - 1][nowy] != 0) {
			vector<int> temp;
			temp.push_back(nowx - 2);
			temp.push_back(nowy);
			return temp;
		}
	}
	if (nowy > 1) {
		if (map[nowx][nowy - 2] == nowwant && board[nowx][nowy - 1] != 0) {
			vector<int> temp;
			temp.push_back(nowx);
			temp.push_back(nowy - 2);
			return temp;
		}
	}
	if (nowx < 6) {
		if (map[nowx + 2][nowy] == nowwant && board[nowx + 1][nowy] != 0) {
			vector<int> temp;
			temp.push_back(nowx + 2);
			temp.push_back(nowy);
			return temp;
		}
	}
	if (nowy < 6) {
		if (map[nowx][nowy + 2] == nowwant && board[nowx][nowy + 1] != 0) {
			vector<int> temp;
			temp.push_back(nowx);
			temp.push_back(nowy + 2);
			return temp;
		}
	}
}

int main() {
	
	std::vector< std::vector<int>> board(8, vector<int>(8,0));
	std::vector< std::vector<int>> step;
	vector<vector<vector<int> > > ChildMove;

	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			cin >> board[i][j];
		}
	}
	cout << endl;
	bool is_black = true;
	ChildMove = ExpandChildMove(board, is_black, 7, 5);
	for(int i=0; i<ChildMove.size(); i++){
		for(int j=0; j<ChildMove[i].size(); j++){
				cout << "( " << ChildMove[i][j][0] << "," << ChildMove[i][j][1] << " )," ;  
		}
		cout << endl;
	}
	return 0;
}

/*
1 2 0 2 0 2 2 0 
1 0 1 0 2 0 0 0
0 2 0 0 0 0 0 0
0 0 1 0 0 1 0 0
0 2 0 0 1 0 0 2 
1 0 2 0 0 0 0 0
0 1 1 0 0 2 0 2
0 0 0 0 0 1 2 0
*/