/*
    PROTOCOL TIMING CONSTRAINS

    #writeOne -> T1H (0.8us = 800ns) + T1L (0.45us = 450ns) >> 1250ns = 1.25us 
    #writeZero -> T0H (0.4us = 400ns) + T0L (0.85us = 850ns) >> 1250ns = 1.25us
    Reset -> T0L (50us).
    #Delay -> 50us.
*/


/*
    16MHz Crystal
    1 cy -> 1/16000000 -> 63ns

    800ns -> 13*cy = 819ns
    450ns -> 7*cy  = 441 ns
    ###-> WriteOne -> T1H(13*cy) + T1L(7*cy) -> T = 1260ns


    850s -> 14*cy = 882ns
    400ns -> 6*cy  = 378 ns
    ###-> WriteOne -> T0H(6*cy) + T0L(14*cy) -> T = 1260ns
*/

/*
    20MHz Crystal
    1 cy -> 1/20000000 -> 50 ns

    700ns -> 16*cy = 800 ns
    550ns -> 9*cy = 450 ns
    ###-> WriteOne  -> T1H(16*cy) + T1L(9*cy) -> T = 1250 ns

    400ns ->  8*cy = 400 ns
    850ns -> 17*cy = 850 ns
    ###-> WriteZero -> T0H(8*cy)  + T0L(17*cy) -> T = 1250 ns
*/


/* WS2812 @16 MHz (approx target):
   0-bit: H≈0.35µs (~6 cyc), L≈0.80µs (~13 cyc)
   1-bit: H≈0.70µs (~11 cyc), L≈0.60µs (~10 cyc)
   Account ~2 cyc for each port write.


*/





