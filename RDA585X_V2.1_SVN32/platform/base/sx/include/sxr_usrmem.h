#ifndef __SXR_USRMEM_H__
#define __SXR_USRMEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __SXR_USRMEM_VAR__
#define DefExtern
#else
#define DefExtern extern
#endif

#define SXR_NB_USER_CLUSTER 32
DefExtern struct
{
 void *Add;
 u16   Size;
 u16   AlignSize;
 u8    NbCluster;
 u8    FirstCluster;
 u16   NbAllocation;
} sxr_UsrCluster [SXR_NB_USER_CLUSTER];

DefExtern u8 sxr_NbUsrCluster;

#ifdef __SXR_RELEASED_PTR_TO_NIL__
void _sxr_UsrFree (void **Ptr);
#define sxr_UsrFree(a)  _sxr_UsrFree((void *)&(a))
#else
void sxr_UsrFree (void *Ptr);
#define _sxr_UsrFree(a) sxr_UsrFree(a)
#endif




void *sxr_UsrMalloc (u32 Size);
void sxr_NewUsrCluster (u16 Size, u16 NbCluster, u8 Idx);

#undef DefExtern

#ifdef __cplusplus
}
#endif

#endif //__SXR_USRMEM_H__
