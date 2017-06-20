/* 
 * File:   MosqCallback.h
 * Author: debian
 *
 * Created on 20. Juni 2017, 14:48
 */

#ifndef MOSQCALLBACK_H
#define	MOSQCALLBACK_H

class MosqCallback {
public:
    virtual void on_mosqEvent(const void* msg) = 0;
};

#endif	/* MOSQCALLBACK_H */

