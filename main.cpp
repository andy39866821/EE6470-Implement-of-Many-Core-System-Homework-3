#define SC_INCLUDE_FX // enable the fixed-point data types

#include <iostream>
#include <string>
#include <systemc>
#include <sys/time.h>
#include "filter.h"
#include "testbench.h"

using namespace std;
using namespace sc_core;
using namespace sc_dt;

int sc_main(int argc, char *argv[]) {
    Testbench tb("tb");
    Filter filt("filt");
    tb.initiator.i_skt(filt.t_skt); //Bind socket 

    cout << "reading bitmap..." << '\t' << sc_time_stamp()<< endl;
    tb.read_bmp();
    
    cout << "starting simulation..." << '\t' << sc_time_stamp()<< endl;

    sc_start();
    
    cout << "finish simulation" << '\t' << sc_time_stamp()<< endl;
    tb.write_bmp();
    cout << "writing bitmap..." << '\t' << sc_time_stamp()<< endl;
    return (0);
}