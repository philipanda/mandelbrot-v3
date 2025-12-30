#ifdef DEBUG_MODE
    #define DEBUG if (1)
#else
    #define DEBUG if (0)
#endif

#ifdef DEBUG_CAMERA
    #define DEBUG_C if (1)
#else
    #define DEBUG_C if (0)
#endif

#define SDL_CHECK_ERROR(value) \
do{ \
	if ((void*)value == NULL){ \
		SDL_Log("Error initializing %s: %s\n", #value, SDL_GetError()); \
		return -1; \
	} \
}while(0);