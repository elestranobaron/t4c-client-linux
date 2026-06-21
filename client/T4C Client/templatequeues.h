// TemplateQueues.h: FIFO queue (Virtual Dreams Library style)
//
#if !defined(AFX_TEMPLATEQUEUES_H__B5F32863_E0F7_11D0_A876_00E029058624__INCLUDED_)
#define AFX_TEMPLATEQUEUES_H__B5F32863_E0F7_11D0_A876_00E029058624__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if defined(_WIN32)
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif
#include <windows.h>
#endif

template <class ObjectType> class TemplateQueue;

template <class ObjectType>
class TemplateQueueObject {
	friend TemplateQueue<ObjectType>;

	private:
		TemplateQueueObject *Greater;

	public:
		TemplateQueueObject(void);
		ObjectType *Pointer;
};

template <class ObjectType>
class TemplateQueue {
	private:
		TemplateQueueObject<ObjectType> *Head;
		TemplateQueueObject<ObjectType> *Tail;
		int nb_objects;

	public:
		TemplateQueue(void);
		~TemplateQueue(void);

		void AddToQueue(ObjectType *obj);
		void AddToQueue(ObjectType &obj);

		ObjectType *Retreive(BOOL boDelete = TRUE);
		int NbObjects(void);
};

template <class ObjectType>
TemplateQueueObject<ObjectType>::TemplateQueueObject(void) : Greater(0) {
}

template <class ObjectType>
TemplateQueue<ObjectType>::TemplateQueue(void) : Head(0), Tail(0), nb_objects(0) {
}

template <class ObjectType>
TemplateQueue<ObjectType>::~TemplateQueue(void) {
	while (Retreive()) {
	}
}

template <class ObjectType>
void TemplateQueue<ObjectType>::AddToQueue(ObjectType *obj) {
	TemplateQueueObject<ObjectType> *TempObject = new TemplateQueueObject<ObjectType>;
	TempObject->Pointer = obj;
	TempObject->Greater = 0;

	if (!Head) {
		Head = Tail = TempObject;
	} else {
		Tail->Greater = TempObject;
		Tail = TempObject;
	}
	nb_objects++;
}

template <class ObjectType>
void TemplateQueue<ObjectType>::AddToQueue(ObjectType &obj) {
	AddToQueue(&obj);
}

template <class ObjectType>
ObjectType *TemplateQueue<ObjectType>::Retreive(BOOL boDelete) {
	if (!Head) {
		return NULL;
	}

	ObjectType *obj = Head->Pointer;

	if (boDelete) {
		TemplateQueueObject<ObjectType> *TempObject = Head;
		Head = Head->Greater;
		if (!Head) {
			Tail = 0;
		}
		delete TempObject;
		nb_objects--;
	}

	return obj;
}

template <class ObjectType>
int TemplateQueue<ObjectType>::NbObjects(void) {
	return nb_objects;
}

#endif // !defined(AFX_TEMPLATEQUEUES_H__B5F32863_E0F7_11D0_A876_00E029058624__INCLUDED_)
