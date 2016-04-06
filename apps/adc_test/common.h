

#ifndef __COMMON_H
#define __COMMON_H

#ifdef	FALSE
#undef	FALSE
#endif
#define FALSE	(0)

#ifdef	TRUE
#undef	TRUE
#endif
#define	TRUE	(1)

#ifdef	NULL
#undef	NULL
#endif
#define NULL	(0)

#ifdef  ON
#undef  ON
#endif
#define ON      (1)

#ifdef  OFF
#undef  OFF
#endif
#define OFF     (0)

typedef enum {RESET = 0, SET = !RESET} FLAGSTATUS, ITSTATUS;

typedef enum {DISABLE = 0, ENABLE = !DISABLE} FUNCTIONALSTATE;
#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))


#endif /* __COMMON_H */
