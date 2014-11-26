//misc functions

bit e_wait;                         //event waiting Begining of Ind Protocol
bit e_check_ind;                    //event check Ind Protocol
bit e_prep_ind;                     //event prepare indication on IND1..3
bit e_prep_sound;                   //event prepare sound
bit e_sound1;                       //event sound1
bit e_sound2;                       //event sound2
bit e_blink;                        //event ind blink
bit e_prep_blink;                   //event prepare blinking
bit e_order2fifo;                   //event save orders to order_fifo[10]
bit e_minT;                         //event check min time for indication to stay
bit e_empty;                        //event check order_fifo empty

bit f_ind;                          //flag ind-s off/on
bit f_minT;                         //flag =1 when min time for ind < 10 sec

unsigned short stime_blink_ln;      //timer blink length in sec
unsigned int stime_blink;           //timer blink period in ms
unsigned int stime_check;           //timer check protocol appeared. check next byte comming to 8 ms
unsigned int stime_s_ln;          //timer length of every sound in sec
unsigned int stime_s;               //timer period of every sound
unsigned short stime_minT;           //timer min steady indication in sec

short rx_i;                         //index for receiving rs485 bytes and flag= -1
short foll_ln;                      //length of protocol following bytes
unsigned short order;               //containing indication byte come from patSt
unsigned short order_temp;          //temp of order var
char ind_str[4];                    //contain character for indication
unsigned short order_fifo[10];      //contain orders come in within 10 sec interval

extern bit e_IND1;                  //ind 1
extern bit e_indoff;                //all indicators off
extern unsigned int stime_ind;      //timer dinamic indication



//---------Refresh Protocol-------------------------
// it refresh on few sec all docSt
// 0x10,X,X,X,X,X,X,X,0x55 -> format
//  |   |<---   --->   |-> END byte = 0x55
//  |         | - - - - - - -> waiting pat , menu1....6 waiting pat (7 bytes)
//  |-> Prot. number , addess all doc stations . docSt1...docStn

//---------Request Protocol---------------------------------------
//request docSt1..docSt15 one by one
// 0xXX,0x55
//   |    |->End Byte
//  0x11-request docSt 1
//  0x12-------------  2
//   ...
//  0x1F-request docSt 15

//---------Responce Protocol--------------------------------------
//response from docSt-n after request from patSt , if NO call->No response
// 0x30,0xXX,0x55
//   |    |    |->END byte
//   |    |->0x10 - call next pat
//   |       0x11 - call next pat from menu 1
//   |       0x12 - call next pat from menu 2
//   |        ..
//   |       0x16 - call next pat from menu 6
//   |       0x20 - repeat call next pat
//   |       0x21 - call prev pat
//   |-> Prot number

//-----------Indication Protocol--------------------------
// send info from patSt -> Indication module to indicate called pat.
// 0x20,0xXX,0x55
//   |    |   |-> END byte
//   |    |-> Ind value
//   |-> Prot number



//--------------power up,reset delay, clear critical outputs----------------------------


void powerup_delay()         //power up,reset delay, clear critical outputs
{   unsigned int stime_ms_PWR;
    unsigned volatile short temp;

    stime_ms_PWR = ms;
    while(t_ms(stime_ms_PWR,500) == 0)      //power up(reset) 500 ms delay
    {   latb=1;                     //set IND1..3 on
        latc.b3=1;
        latc.b4=1;
        latc.b5=1;

        temp=RCREG1;         //clear Rx buffer
    }
    
    latb=1;                     //Clear IND1..3
    latc.b3=1;
    latc.b4=1;
    latc.b5=1;
}


 //--------clear order fifo-----------------------------------------------------

void clear_order_fifo()         //clear order_fifo[10] with 0-s
{   unsigned short i;
    
    for(i=0;i<sizeof(order_fifo);i++)
        order_fifo[i]=0;
}


//-------------Receive Order Protocol or empty Rx buffer for all other protocols----------



void _wait()      //check if recognized protocol appear and start  _check_refr() , clear Rx reg if not
{   unsigned volatile short temp;

    if(PIR1.RC1IF == 1)         //if 1-st byte arrive on rs485 line (addr)
    {   temp=RCREG1;            //read 1-st byte

        if(temp == 0x20)        //check it's Ind Pritocol
        {   e_wait=0;
            e_check_ind=1;
            stime_check = ms;       //start timer check ind prot
        }
    }
}




void _check_ind()       //check Ind Protocol,check timing between bytes(<8ms),check 2-nd byte=0,check END byte
{                       //if it is correct-> set 'order' var
    unsigned short temp;

    if(t_ms(stime_check,20))     //if 20 ms pass and no all ind prot received
    {   e_check_ind=0;
        e_wait=1;
        return;                 //this is not protocol
    }else
    {   if(PIR1.RC1IF == 1)     //if next byte arrive
        {   temp=RCREG1;        //read next incoming byte

            if(rx_i==-1)        //if it is 2-nd byte
            {   order_temp = temp;
                rx_i++;
            }else
            {   if(temp==0x55)      //if END byte all bytes read successfuly->set env. for new protocol
                {   if(order!=order_temp)
                    {   order = order_temp;
                        e_order2fifo=1;         //save orders to order_fifo[10]
                    }
                        
                    rx_i = -1;          //set flag = -1 for the next incoming protocol
                    e_check_ind=0;      //clear event
                    e_wait=1;           //resume watching for incoming protocol
                }else                   //this is not protocol
                {   rx_i = -1;          //set flag = -1 for the next incoming protocol
                    e_check_ind=0;      //clear event
                    e_wait=1;           //resume watching for incoming protocol
                }
            }
        }
    }
}



//---------Save orders to order_fifo[10]---------------------------------------



void order2fifo()           //Save orders to order_fifo[10]
{   unsigned short i;

    for(i=0;i<sizeof(order_fifo);i++)
    {   if(order_fifo[i]==0)
        {   order_fifo[i]=order;
            break;
        }
    }
    
    e_order2fifo=0;
}



//---------Check order_fifo[] empty--------------------------------------------


void _empty()     //Check non stop order_fifo[] empty ,set/clear event e_empty
{   if(order_fifo[0]!=0)
        e_empty=0;      //order_fifo is not empty
    else
        e_empty=1;      //order_fifo is empty
}



//---------Change Number on Indication------------------------------------------



void change_ind()           //check 10 sec pass for steady indication , move all order_fifo[] element to the left
{   unsigned short i;

    ByteToStr(order_fifo[0],ind_str);       //convert order_fifo to string ind_str[]
    f_minT=1;                   //flag =1 when min time for ind < 10 sec
    stime_minT=sec;             //start timer min time for indication
    e_minT=1;                   //event chack min time for ind
    e_prep_sound=1;
    e_prep_blink=1;             //start blinking if new digit arear
    
    for(i=1;i<sizeof(order_fifo);i++)
    {   order_fifo[i-1]=order_fifo[i];  }      //move all arr element to the left

    order_fifo[9]=0;
}




void _minT()
{   if(t_sec(stime_minT,10))
    {   f_minT=0;
        e_minT=0;
    }
}



//-----------------------blinking ing---------------------------------------



void prep_blink()
{   e_prep_blink=0;

    e_blink=1;              //start blinking
    stime_blink = ms;       //start timer blink peroid in ms
    stime_blink_ln = sec;   //timer lengt blinking in sec
}



void _blink()
{   if(t_sec(stime_blink_ln,5))     //blinking for 5 sec
    {   e_blink=0;                  //stop blinking
        f_ind=0;                    //drive latb on dynamicInd.h->setLat()
    }else
    {   if(t_ms(stime_blink,200))
        {   if(f_ind==0)
                f_ind=1;
            else
                f_ind=0;
                
            stime_blink=ms;         //restart blinking
        }
    }
}



//-------------------sound func----------------------------------------------



void prep_sound()       //prepare timers and events for sound
{   e_prep_sound=0;
    stime_s_ln=ms;     //length of sound1 or sound2
    stime_s=ms;         //period of sound1/sound2 (2 and 3 ms)
    e_sound1=1;         //event start sound1
    
    lata.b4=1;          //set +sine
}



void _play_sound1()                     //play sound 1 for wev sec
{   if(t_ms(stime_s_ln,300))             //ms play sound1
    {   e_sound1=0;                     //clear soun1 event
        e_sound2=1;                     //set sound2 event
        stime_s_ln=ms;                 //start next sound for fev sec
        stime_s=ms;                     //start period of next sound (2 or 3 ms)
    }else
    {   if(t_ms(stime_s,2))             //period few ms for sound 1
        {   if(lata.b4==1)
                lata.b4=0;              //play -sine
            else
                lata.b4=1;              //play +sine
            
            stime_s=ms;             //start timer for new 2 ms
        }
    }
}


void _play_sound2()                     //play sound 2 for wev sec
{   if(t_ms(stime_s_ln,500))             //1 sec play sound2
    {   e_sound2=0;                     //clear sound2 event
        lata.b4=0;
    }
    else
    {   if(t_ms(stime_s,3))             //period few ms for sound 2
        {   if(lata.b4==1)
                lata.b4=0;              //play -sine
            else
                lata.b4=1;              //play +sine
            
            stime_s=ms;             //start timer for new 2 ms
        }
    }
}
