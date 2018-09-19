//
// Created by root on 9/17/18.
//

#ifndef TRACER_IFACE_H
#define TRACER_IFACE_H

#define DECLARE_INTERFACE(if_struct) \
bool if_struct##_interface(struct if_struct *dst, const struct if_struct *src, struct if_struct **ref, size_t size);\
void if_struct##_neutralise(struct if_struct **ref);


#define DEFINE_INTERFACE(if_struct)\
bool if_struct##_interface(struct if_struct *const dst, const struct if_struct *const src, struct if_struct **const ref, const size_t size) {\
	if (size != sizeof(struct if_struct)) { return false; }\
	if (*ref) { return false; }\
	memcpy(dst, src, sizeof(struct if_struct));\
	*ref = dst;\
	return true;\
}\
void if_struct##_neutralise(struct if_struct **const ref) {\
	if(*ref) {\
    	memcpy(ref, &neutral_##if_struct, sizeof(struct if_struct));\
		*ref = 0;\
    }\
}

#endif //TRACER_IFACE_H
