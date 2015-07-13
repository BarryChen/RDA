

#ifndef __CSW_MSG_H__
#define __CSW_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif



typedef struct 
{
   u32    Id ;        // Identifier Unique for whole stack
   
}MsgHead_t;


typedef struct 
{
   MsgHead_t   H;
   MsgBody_t   B;

}Msg_t;




#ifdef __cplusplus
}
#endif

#endif

