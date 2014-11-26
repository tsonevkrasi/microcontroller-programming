//dinamic indication on IND1..3

bit e_IND1;        //ind 1
bit e_IND2;        //ind 2
bit e_IND3;
bit e_indoff;      //all indicators off

unsigned int stime_ind;     //timer dinamic indication

extern char ind_str[4];     //contain character for indication
extern bit f_ind;           //flag ind-s off/on





void setLat(unsigned short ind_str)      //set LatC who drive indicators IND1 and IND2
{   
    if(f_ind==1)
    {   latb = 0b11111111;       //indicator off
        return;
    }

    switch (ind_str)     //set indicator pins ,ind_str[]->global var host misc.h
    {         //ing pins ->  gfedcba
              //             |||||||
        case 0x30:  latb = 0b10000001;       //"0"
                    break;
        case 0x31:  latb = 0b11110011;       //"1"
                    break;
        case 0x32:  latb = 0b01001001;       //"2"
                    break;
        case 0x33:  latb = 0b01100001;       //"3"
                    break;
        case 0x34:  latb = 0b00110011;       //"4"
                    break;
        case 0x35:  latb = 0b00100101;       //"5"
                    break;
        case 0x36:  latb = 0b00000101;       //"6"
                    break;
        case 0x37:  latb = 0b11110001;       //"7"
                    break;
        case 0x38:  latb = 0b00000001;       //"8"
                    break;
        case 0x39:  latb = 0b00100001;       //"9"
                    break;
        default:    latb = 0b11111111;       //indicator off
    }
}




void _ind1()       //make delay for dinamic indication on IND1 and prepare next indicator IND2
{   if(t_ms(stime_ind,3))
    {   setLat(ind_str[0]);     //set IND1 pins ,ind_str[]->global var host misc.h
        latc.b5 = 0;                //TI1 on
        latc.b4 = 1;                //TI2 off
        latc.b3 = 1;                //TI3 off

        stime_ind = ms;
        e_IND1 = 0;
        e_IND2 = 1;
        e_indoff = 1;
    }
}



void _ind2()       //make delay for dinamic indication on IND2 and all indicators off
{   if(t_ms(stime_ind,3))
    {   setLat(ind_str[1]);     //set IND1 pins ,ind_str[]->global var host misc.h
        latc.b5 = 1;                //TI1 off
        latc.b4 = 0;                //TI2 on
        latc.b3 = 1;                //TI3 off

        stime_ind = ms;
        e_IND2 = 0;
        e_IND3 = 1;
        e_indoff = 1;
    }
}


void _ind3()       //make delay for dinamic indication on IND2 and all indicators off
{   if(t_ms(stime_ind,3))
    {   setLat(ind_str[2]);     //set IND1 pins ,ind_str[]->global var host misc.h
        latc.b5 = 1;                //TI1 off
        latc.b4 = 1;                //TI2 off
        latc.b3 = 0;                //TI3 on

        stime_ind = ms;
        e_IND3 = 0;
        e_IND1 = 1;
        e_indoff = 1;
    }
}



void _indoff()          //make delay all indicators off and prepare next led diod DL1...7
{   if(t_ms(stime_ind,1))
    {   latc.b5 = 1;                //TI1 off
        latc.b4 = 1;                //TI2 off
        latc.b3 = 1;                //TI3 off

        stime_ind = ms;
        e_indoff = 0;
    }
}


