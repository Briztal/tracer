//
// Created by root on 9/17/18.
//

#ifndef TRACER_IFACE_H
#define TRACER_IFACE_H


#define DECLARE_INTERFACE(if_struct) \
bool if_struct##_interface(struct if_struct *dst, const struct if_struct *src, size_t size);\
void if_struct##_neutralise(struct if_struct *dst);


#define DEFINE_INTEFACE(if_struct)\
bool if_struct##_interface(struct if_struct *dst, const struct if_struct *src, size_t size) {\
	if (size != sizeof(struct if_struct)) { return false; }\
	memcpy(dst, src, sizeof(struct if_struct));\
	return true;\
}\
void if_struct##_neutralise(struct if_struct *dst) {\
	memcpy(dst, &neutral_##if_struct, sizeof(struct if_struct));\
}

#endif //TRACER_IFACE_H
