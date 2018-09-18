//
// Created by root on 9/17/18.
//

#ifndef TRACER_IFACE_H
#define TRACER_IFACE_H

#define DECLARE_INTERFACE(if_struct) \
struct if_struct##_ref {struct if_struct iface; struct if_struct *ref;};\
void if_struct##_interface(struct if_struct *dst, struct if_struct##_ref *ref, size_t size);\
void if_struct##_neutralise(struct if_struct##_ref *ref);


#define DEFINE_INTEFACE(if_struct)\
void if_struct##_interface(struct if_struct *const dst, struct if_struct##_ref *const ref, const size_t size) {\
	if (size != sizeof(struct if_struct)) { return; }\
	memcpy(dst, &ref->iface, sizeof(struct if_struct));\
	ref->ref = dst;\
}\
void if_struct##_neutralise(struct if_struct##_ref *ref) {\
	if(ref->ref) {\
    	memcpy(ref, &neutral_##if_struct, sizeof(struct if_struct));\
		ref->ref = 0;\
    }\
}

#endif //TRACER_IFACE_H
