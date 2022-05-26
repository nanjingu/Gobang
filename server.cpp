#include <brpc/server.h>
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

namespace example{
class EchoServiceImpl : public myService{
public:
    EchoServiceImpl(){
        memset(board, '*', sizeof(board));
        for(int i = 1; i < SIZE; ++i){
            board[0][i] = '0' + i;
            board[i][0] = '0' + i;
        }
    }
    virtual ~EchoServiceImpl(){}
    virtual void game(::google::protobuf::RpcController* controller,
                       const ::example::myRequest* request,
                       ::example::myResponse* response,
                       ::google::protobuf::Closure* done){

        brpc::ClosureGuard done_guard(done);
        brpc::Controller* cntl =
            static_cast<brpc::Controller*>(controller);

        ::google::protobuf::int32 num1 = request->num1(), num2 = request->num2();
        cout <<"Received request[log_id=" << cntl->log_id() 
                  << "] from " << cntl->remote_side() 
                  << " to " << cntl->local_side()
                  << ": " << num1 << " " << num2 
                  << endl;
        board[num1][num2] = '@';
        for(int i = 0; i < SIZE; ++i){
            for(int j = 0; j < SIZE; ++j){
                cout << board[i][j]<<' ';
            }
            cout<<endl;
        }
        if(checkKey(num1, num2, '@')){
            cout<<"YOU LOSE!!!"<<endl;
            return;
        }
        else cout<<"your(#) turn: "<<endl;
        cin >> num1 >> num2;
        while(num1>SIZE||num1<0 || num2>SIZE||num2<0){
            cout<<"your(#) input out of bounds, please re-input: "<<endl;
            cin >> num1 >> num2;
        }
        board[num1][num2] = '#';
        for(int i = 0; i < SIZE; ++i){
            for(int j = 0; j < SIZE; ++j){
                cout << board[i][j]<<' ';
            }
            cout<<endl;
        }
        if(checkKey(num1, num2, '#')){
            cout<<"YOU WIN!!! "<<"Please input ctrl+C to quit";
        }
        else cout<<"rival turn: "<<endl;
        response->set_num1(num1);
        response->set_num2(num2);
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
    char board[SIZE][SIZE];
};
}

int main(){
    brpc::Server server;

    example::EchoServiceImpl myImpl;

    if (server.AddService(&myImpl, 
                          brpc::SERVER_DOESNT_OWN_SERVICE) != 0) {
        LOG(ERROR) << "Fail to add service";
        return -1;
    }

    brpc::ServerOptions options;
    options.idle_timeout_sec = 200;
    if (server.Start(8000, &options) != 0) {
        LOG(ERROR) << "Fail to start EchoServer";
        return -1;
    }

    server.RunUntilAskedToQuit();
    return 0;
}