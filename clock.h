/*
this module create global variables ms,sec,min,hours which start incrementing
when procesor is powered.
it include software timer functions t_ms,t_sec,t_minutes,t_hours
Note -> no logic in case of reset situations
Note -> no logic for different MPU and Frequency
*/



unsigned int ms;           //mlseconds -> 2 bytes FFFF (32536) , max -> 999 ms
unsigned short sec;        //seconds  -> 1 byte FF (255) , max -> 59 sec
unsigned short minutes;    //minutes -> max 59 min
unsigned short hours;      //hours -> max 255 hours




void startClock()
{   //clear global var, initialize and start timer 0
    //f 16Mhz -> Fosc/4 = 4Mhz /32 = 125000Hz =0.008ms -> timer must count 125 times to reach 1ms
    //we load TMR0L reg with 131 (131+125=256(0)) to reach interupt on 1 ms
    
    ms=0;
    sec=0;
    minutes=0;
    hours=0;

    INTCON.TMR0IE = 1;  //enables the TMR0 overflow interrupt
    INTCON.TMR0IF = 0;  //clear TMR0 Overflow Interrupt Flag bit, must be cleared by software
    INTCON2.TMR0IP = 1; //Overflow Interrupt - high priority
    
    TMR0L = 131;        //to reach interrupt on 1 ms
    T0CON = 0b11000100;  //1->start timer,1->8 bit,0->internal Clock Source Select,0->ext clock edge,
                        //0->prescaler on(???-document diagram error),100->1:32
}





void interrupt()
{   //fill up variables with 999 ms, 59 sec, 59 min, 255 hours
    TMR0L = 131;        //reload timer to reach interrupt on 1 ms

    ms = ms+1;
    if(ms > 999)
    {  ms = 0;
       sec = sec + 1;
       if(sec > 59)
       {  sec = 0;
          minutes = minutes + 1;
          if(minutes > 59)
          {  minutes = 0;
             hours = hours + 1;       //in case FF+1=00 start to count from 00 again
          }
       }
    }

    INTCON.TMR0IF = 0;    //clear TMR0 Overflow Interrupt Flag bit, must be cleared by software
}







//soft timer ms (2 bytes). max 999 ms. return 1 if delay in ms reached ,else 0 , stime = start time
unsigned short t_ms(unsigned int stime,unsigned int delay)
{   if(ms != stime)          //check in case ms==stime
    {   if(ms > stime)
        {   if((stime+delay) <= ms)
                return 1;
            else
                return 0;
        }else
            if((stime+delay) <= (1000+ms))
                return 1;
            else
                return 0;
    }else
        return 0;
}



//soft timer seconds (1 byte). max 59 sec . "result"=1 if delay in sec reached else =0 , stime = start time
unsigned short t_sec(unsigned short stime,unsigned short delay)
{   if(sec != stime)              //check in case sec==stime
    {   if(sec > stime)
        {   if((stime+delay) <= sec)
                return 1;
            else
                return 0;
        }else
            if((stime+delay) <= (60+sec))
                return 1;
            else
                return 0;
    }else
        return 0;
}



//soft timer minutes (1 byte). max 59 min. "result"=1 if delay in min reached else =0 , stime = start time
unsigned short t_minutes(unsigned short stime,unsigned short delay)
{   if(minutes != stime)         //check in case min==stime
    {   if(minutes > stime)
        {   if((stime+delay) <= minutes)
                return 1;
            else
                return 0;
        }else
            if((stime+delay) <= (60+minutes))
                return 1;
            else
                return 0;
    }else
        return 0;
}



//sot timer hours (2 bytes). max 255 hours. "result"=1 if delay in min reached else =0 , stime = start time
unsigned short t_hours(unsigned short stime,unsigned short delay)
{   if(hours != stime)              //check in case hours==stime
    {   if(hours > stime)
        {   if((stime+delay) <= hours)
                return 1;
            else
                return 0;
        }else
            if((stime+delay) <= (256+hours))
                return 1;
            else
                return 0;
    }else
        return 0;
}
