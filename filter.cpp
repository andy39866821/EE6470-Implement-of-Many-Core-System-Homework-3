#include "filter.h"

Filter::Filter(sc_module_name n)
    : sc_module(n), t_skt("t_skt"){
        
    SC_THREAD(convolution);
    t_skt.register_b_transport(this, &Filter::blocking_transport);
}

void Filter::convolution(){
    sc_uint<8> source_r, source_g, source_b, i ,j;
    sc_fixed<32, 24>  R, G, B;      // color of R, G, B

    while(1) {
        R = G = B = 0;
        for (i=0 ; i!=filterHeight ; ++i) {
            for (j=0 ; j!=filterWidth ; ++j) {
                source_r = i_r.read();
                source_g = i_g.read();
                source_b = i_b.read();

                R += (sc_fixed<32, 24>)source_r * filter[i][j];
                G += (sc_fixed<32, 24>)source_g * filter[i][j];
                B += (sc_fixed<32, 24>)source_b * filter[i][j];
                
            }
        }
        o_r.write((sc_uint<8>)(R/factor));
        o_g.write((sc_uint<8>)(G/factor));
        o_b.write((sc_uint<8>)(B/factor));
    }
    
}

void Filter::blocking_transport(tlm::tlm_generic_payload &payload, sc_core::sc_time &delay) {
    sc_dt::uint64 addr = payload.get_address();
    //addr = addr - base_offset;
    unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    unsigned char *data_ptr = payload.get_data_ptr();
    sc_uint<8> r, g, b;
    switch (payload.get_command()) {
        case tlm::TLM_READ_COMMAND:
            switch (addr) {
                case FILTER_RESULT_ADDR:
                    r = o_r.read();
                    g = o_g.read();
                    b = o_b.read();
                    break;
                default:
                    std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                            << std::setfill('0') << std::setw(8) << std::hex << addr
                            << std::dec << " is not valid" << std::endl;
                    break;
            }
            data_ptr[0] = r;
            data_ptr[1] = g;
            data_ptr[2] = b;
            data_ptr[3] = 0;
            break;
        case tlm::TLM_WRITE_COMMAND:
            switch (addr) {
                case FILTER_R_ADDR:
                    if (mask_ptr[0] == 0xff)
                        i_r.write(data_ptr[0]);
                    if (mask_ptr[1] == 0xff)
                        i_g.write(data_ptr[1]);
                    if (mask_ptr[2] == 0xff)
                        i_b.write(data_ptr[2]);
                    break;
                default:
                    std::cerr << "Error! SobelFilter::blocking_transport: address 0x"
                            << std::setfill('0') << std::setw(8) << std::hex << addr
                            << std::dec << " is not valid" << std::endl;
                    break;
            }
            break;
        case tlm::TLM_IGNORE_COMMAND:
            payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            return;
        default:
            payload.set_response_status(tlm::TLM_GENERIC_ERROR_RESPONSE);
            return;
    }
    payload.set_response_status(tlm::TLM_OK_RESPONSE); // Always OK
}

