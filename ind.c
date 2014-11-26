//Indication main program
//control elctronic board "Indication"
//microC600, PIC18F25K22, 16 Mhz Inernal clock

#include "clock.h"
#include "misc.h"
#include "dynamicInd.h"






void main()
{
    #include "Init.h"
    startClock();               //start time counting software clock. (host clock.h)

     
    e_IND1 = 1;
    e_IND2 = 0;
    e_IND3 = 0;
    e_indoff = 0;
    e_wait=1;                   //event waiting Begining of Protocol
    e_check_ind=0;              //event check Order Protocol
    e_prep_sound=0;             //event prepare sound
    e_sound1=0;                 //event sound1
    e_sound2=0;                 //event sound2
    e_blink=0;                  //event blink ind
    e_prep_blink=0;             //event orepare blinking
    e_order2fifo=0;             //event save orders to order_fifo[10]
    e_minT=0;                   //event check min time for indication to stay
    e_empty=0;                  //event check order_fifo empty

    f_ind=0;                    //flag ind-s off/on
    f_minT=0;                   //flag =1 when min time for ind < 10 sec

    stime_ind=ms;               //timer ind

    rx_i=-1;                    //index for receiving rs485 bytes and flag= -1
    foll_ln=0;                  //length of protocol following bytes
    order=0;                    //contain number to be indicated on IND1..3
    ind_str[0]=0x30;            //= "0"
    ind_str[1]=0x30;
    ind_str[2]=0x30;
    
    


    powerup_delay();            //power up,reset delay, clear critical outputs
    clear_order_fifo();         //clear order_fifo[10] with 0-s



    while(1)
    {
 //--------dinamic indication on DL1...7   IND1  and  IND2---------------------
        if(e_IND1==1 && e_indoff==0)
            _ind1();
        if(e_IND2==1 && e_indoff==0)
            _ind2();
        if(e_IND3==1 && e_indoff==0)
            _ind3();
        if(e_indoff==1)
            _indoff();
 //---------Ind blinking-------------------------------------------------------
        if(e_prep_blink==1)
           prep_blink();
        if(e_blink==1)
           _blink();
 //---------Reseive Ind Protocol for IND1..3-----------------------------------
        if(e_wait==1)
            _wait();                //check if recognized protocol appear or clear Rx line
        if(e_check_ind==1)
            _check_ind();           //check the whole Ind Order format is correct
 //---------Save orders to order_fifo[10]---------------------------------------
        if(e_order2fifo==1)
            order2fifo();           //Save orders to order_fifo[10]
 //---------Check non stop order_fifo[] empty --------------------------------------------
        _empty();                   //Check order_fifo[] empty
 //---------Change Indication with new number-----------------------------------
        if(f_minT==0 && e_empty==0)
            change_ind();           //check 10 sec pass for indication
        if(e_minT==1)
            _minT();                 //check min time to indicate new number
 //---------Play sound1 and sound2 for few sec----------------------------------
        if(e_prep_sound==1)
            prep_sound();           //prepare timers ans events for sound
        if(e_sound1==1)
            _play_sound1();         //play sound with 2 ms period (T)
        if(e_sound2==1)
            _play_sound2();         //play sound with 3 ms period (T)
            
    }
}
