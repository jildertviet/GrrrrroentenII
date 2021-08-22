//
//  ipv6Scanner.h
//  batmanBridgeSetup
//
//  Created by Jildert Viet on 03-09-18.
//

#ifndef ipv6Scanner_h
#define ipv6Scanner_h

#include "ofMain.h"

class ipScanner: public ofThread {
public:
    ipScanner(){
        startThread();
    };
    ~ipScanner(){
        ipStatusChannel.close();
        waitForThread(true);
    };
    bool bStop = false;
    vector<string> ips;
    
    vector<bool> getActiveIPs(){
        ipStatusChannel.tryReceive(ipStatusList);
        return ipStatusList;
    }
private:
    void threadedFunction(){
        // wait until there's a new frame
        // this blocks the thread, so it doesn't use
        // the CPU at all, until a frame arrives.
        // also receive doesn't allocate or make any copies
        
        while(!bStop){
            vector<bool> foundIPs = {0, 0, 0, 0, 0};
            for(char i=0; i<ips.size(); i++){
                string cmd = "ping ";
                cmd += ips[i];
                cmd += " -c 1 -t 1";
                string res = ofSystem(cmd);
                if(ofStringTimesInString(res, "bytes from ") != 0){
//                    cout << ips[i] << " is online" << endl;
                    foundIPs[i] = true;
                }
            }
            
#if __cplusplus>=201103
            ipStatusChannel.send(std::move(foundIPs));
#else
            ipStatusChannel.send(foundIPs);
#endif
            ofSleepMillis(4000);
        }
    }
    ofThreadChannel<vector<bool>> ipStatusChannel;
    vector<bool> ipStatusList;
};

#endif /* ipv6Scanner_h */
