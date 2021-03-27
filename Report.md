# EE6470 Homework 3 Report
###### tags: `EE6470`
## Problems and Solutions
### Data transfer method: union
Q: **union** datatype is redundent in gaussian blur
Since filter recieve R, G, and B form testbench, then reply R, G, and B to testbench, memory space sharing in **union** is redundent.
A: Use **char** array with 4 elements is enough.
```
unsigned char buufer[4];
unsigned char mask[4];
// Prepare data
buufer[0] = R;
buufer[1] = G;
buufer[2] = B;
mask[0] = 0xff;
mask[1] = 0xff;
mask[2] = 0xff;
mask[3] = 0;
// send data by socket
initiator.write_to_socket(FILTER_R_ADDR, mask, buufer, 4);
```
## Implementation details 


### Part 1
I modified lab03: sobel filter by socket to complete this part.
#### Block diagram
![](https://i.imgur.com/TF99fxH.jpg)

#### filter parameters
I copy them from [here](https://lodev.org/cgtutor/filtering.html#Gaussian_Blur_), which is TA provied.

```
double filter[filterHeight][filterWidth] = {
  {0.077847, 0.123317, 0.077847},
  {0.123317, 0.195346, 0.123317},
  {0.077847, 0.123317, 0.077847}
};

```
#### convolution function
The convolution part is same to homework 1&2, after RGB calculation, I delcared **buffer** and **mask** to store the result then send to **blocking trasport** by **sc_fifo**
```
    o_r.write((sc_uint<8>)(R/factor));
    o_g.write((sc_uint<8>)(G/factor));
    o_b.write((sc_uint<8>)(B/factor));
}
```

#### blocking_transport: sending part
**blocking_transport** function recieve the data form convolution part, then pack them to be sent to **testbench**.
```
    sc_dt::uint64 addr = payload.get_address();
    unsigned char *mask_ptr = payload.get_byte_enable_ptr();
    unsigned char *data_ptr = payload.get_data_ptr();
    .....
    r = o_r.read();
    g = o_g.read();
    b = o_b.read();
    .....
    data_ptr[0] = r;
    data_ptr[1] = g;
    data_ptr[2] = b;
    data_ptr[3] = 0;
    break;

```
#### blocking_transport: recieving part
**blocking_transport** function recieve the data form **testbench**, then send them to **filter** for convolution.
```
    if (mask_ptr[0] == 0xff)
        i_r.write(data_ptr[0]);
    if (mask_ptr[1] == 0xff)
        i_g.write(data_ptr[1]);
    if (mask_ptr[2] == 0xff)
        i_b.write(data_ptr[2]);

```
#### Testbench: sending part
After the RGB data to be sent to **filter** is prepared, **testbench** will pack them into a 4-bytes buffer then set it into socket transfering function.
```
    // Prepare data
    buufer[0] = R;
    buufer[1] = G;
    buufer[2] = B;
    mask[0] = 0xff;
    mask[1] = 0xff;
    mask[2] = 0xff;
    mask[3] = 0;
    // send data by socket
    initiator.write_to_socket(FILTER_R_ADDR, mask, buufer, 4);
```
#### Testbench: recieving part
First it will read the mask and buffer from sokcet, then access the data by pointer for image writing out.
```
    initiator.read_from_socket(FILTER_RESULT_ADDR, mask, buffer, 4);

    *(image_t + byte_per_pixel * (width * y + x) + 2) = buffer[0];
    *(image_t + byte_per_pixel * (width * y + x) + 1) = buffer[1];
    *(image_t + byte_per_pixel * (width * y + x) + 0) = buffer[2];
```

## Additional features
### Simplier transfering data structure
In this homework, I didn't use **union** as transferring data type. I just use 4-bytes **char** array to hold the data when transferring.

## Experimental results
###  masking usage
For the **mask** we transfered between **filter** and **testbench**, I try to use and not use the masking value when access data, then I get the same result, that is, if we can confirm the transferring data format, we may not need to transfer masking value to save bandwidth.
#### Not used masking value
```
    i_r.write(data_ptr[0]);
    i_g.write(data_ptr[1]);
    i_b.write(data_ptr[2]);
```
![](https://i.imgur.com/KciAqY1.png)

#### used masking value
```
   if (mask_ptr[0] == 0xff)
        i_r.write(data_ptr[0]);
    if (mask_ptr[1] == 0xff)
        i_g.write(data_ptr[1]);
    if (mask_ptr[2] == 0xff)
        i_b.write(data_ptr[2]);

```
![](https://i.imgur.com/KciAqY1.png)

## Discussions and conclusions
Since this homework only have one-to-one data transferring, we may not found the difference between pure read/write of sc_fifo and socket transferring. If we have multiple writer, we should consider the data recieve ordering, that is, the multiplexer of recieving data in socket is more easy to implement rather than pure sc_fifo transferring.