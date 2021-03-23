#define SC_INCLUDE_FX // enable the fixed-point data types
#include <systemc>
#include <sys/time.h> 
#include <cmath>
#include <iomanip>

#include "tlm"
#include "tlm_utils/simple_target_socket.h"
#include "filter_def.h"

using namespace sc_core;
using namespace sc_dt;

class Filter: public sc_module {
private:

    sc_uint<8> filterWidth = 3;
    sc_uint<8> filterHeight = 3;
    sc_fixed <32, 24> filter[3][3] = {
        {0.077847, 0.123317, 0.077847},
        {0.123317, 0.195346, 0.123317},
        {0.077847, 0.123317, 0.077847}
    };
    sc_fixed <32, 24> factor = 1;


    void blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay);
    void convolution();
public:
    tlm_utils::simple_target_socket<Filter> t_skt;
    sc_fifo<sc_uint<8> > o_r;
    sc_fifo<sc_uint<8> > o_g;
    sc_fifo<sc_uint<8> > o_b;
    sc_fifo<sc_uint<8> > i_r;
    sc_fifo<sc_uint<8> > i_g;
    sc_fifo<sc_uint<8> > i_b;

    SC_HAS_PROCESS(Filter);
    Filter(sc_module_name n);
    ~Filter() = default;
   
};