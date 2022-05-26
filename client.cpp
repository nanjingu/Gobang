#include <brpc/channel.h>
#include <string>
#include <iostream>
#include "mytest.pb.h"

using namespace std;
#define SIZE 10
enum dire
{
    UP = 0,
    UPLEFT,
    LEFT,
    LEFTDOWN,
    DOWN,
    DOWNRIGHT,
    RIGHT,
    RIGHTUP
};


class goBang{
public:
    goBang(){
        memset(board, '*', sizeof(board));
        for(int i = 1; i < SIZE; ++i){
            board[0][i] = '0' + i;
            board[i][0] = '0' + i;
        }
    }
    inline int check(int row, int col, int dir, char ch){
        int result = 0;
        switch(dir){
            case 0:while(row>=0&&row<=SIZE&&col>=0&&col<=SIZE && board[row][col] == ch){
                ++result;
                --col;
            }
            break;
            case 1:while(row>=0&&row<=SIZE&&col>=0&&col<=SIZE && board[row][col] == ch){
                ++result;
                --row;
                --col;
            }
            break;
            case 2:while(row>=0&&row<=SIZE&&col>=0&&col<=SIZE && board[row][col] == ch){
                ++result;
                --col;
            }
            break;
            case 3:while(row>=0&&row<=SIZE&&col>=0&&col<=SIZE && board[row][col] == ch){
                ++result;
                ++row;
                --col;
                break;
            }
            case 4:while(row>=0&&row<=SIZE&&col>=0&&col<=SIZE && board[row][col] == ch){
                ++result;
                ++row;
            }
            break;
            case 5:while(row>=0&&row<=SIZE&&col>=0&&col<=SIZE && board[row][col] == ch){
                ++result;
                ++row;
                ++col;
            }
            break;
            case 6:while(row>=0&&row<=SIZE&&col>=0&&col<=SIZE && board[row][col] == ch){
                ++result;
                ++col;
            }
            break;
            case 7:while(row>=0&&row<=SIZE&&col>=0&&col<=SIZE && board[row][col] == ch){
                ++result;
                --row;
                ++col;
            }
            break;
        }
        return result;
    }
    inline bool checkKey(int row, int col, char ch){
        if(check(row-1, col,   UP,       ch) + check(row+1, col,   DOWN,      ch) >= 4) return true;
        if(check(row-1, col-1, UPLEFT,   ch) + check(row+1, col+1, DOWNRIGHT, ch) >= 4) return true;
        if(check(row,   col-1, LEFT,     ch) + check(row,   col+1, RIGHT,     ch) >= 4) return true;
        if(check(row+1, col-1, LEFTDOWN, ch) + check(row-1, col+1, RIGHTUP,   ch) >= 4) return true;
        return false;
    }
    void printfBoard(){
        for(int i = 0; i < SIZE; ++i){
            for(int j = 0; j < SIZE; ++j){
                cout << board[i][j]<<' ';
            }
            cout<<endl;
        }
    }
    void yourTune(int& row, int& col){
        cout<<"your(@) turn: "<<endl;
        cin >> row >> col;
        while(row>SIZE||row<0 || col>SIZE||col<0){
            cout<<"your(@) input out of bounds, please re-input: "<<endl;
            cin >> row >> col;
        }
        board[row][col] = '@';
        printfBoard();
        cout<<"rival turn......"<<endl;
    }
    char board[SIZE][SIZE];
};


int main(){
    goBang myboard;
    brpc::Channel channel;
    
    // Initialize the channel, NULL means using default options.
    brpc::ChannelOptions options;
    options.protocol = "baidu_std";
    options.timeout_ms = -1/*milliseconds*/;
    options.max_retry = 3;
    if (channel.Init("0.0.0.0:8000", "", &options) != 0) {
        LOG(ERROR) << "Fail to initialize channel";
        return -1;
    }

    example::myService_Stub stub(&channel);
    int log_id = 0;

    myboard.printfBoard();
    while (1) {
        example::myRequest request;
        example::myResponse response;
        brpc::Controller cntl;
        
        ::google::protobuf::int32 row, col;
        myboard.yourTune(row, col);
        if(myboard.checkKey(row, col, '@')){
            cout<<"YOU WIN!!!"<<endl<<"Please input ctrl+C to quit.";
        }

        request.set_num1(row);
        request.set_num2(col);
        cntl.set_log_id(log_id ++);  
        stub.game(&cntl, &request, &response, nullptr);
        
        if (!cntl.Failed()) {
            row = response.num1();
            col = response.num2();
            myboard.board[row][col] = '#';
            cout << "Received result from " << cntl.remote_side()
                << " to " << cntl.local_side()
                << ": " << row << ' ' << col 
                << " latency=" << cntl.latency_us() << "us" << endl;
            myboard.printfBoard();
            if(myboard.checkKey(row, col, '#')){
                cout<<"YOU LOSE!!!"<<endl;
                return 0;
            }
        } else {
            cout << cntl.ErrorText();
        }
    }

    cout << "EchoClient is going to quit";
    return 0;
}