/* ------------------------------------------------------
   -- File: GO_TAB.H
   --
   --       Goose Implementation
   --          (c) 1997
   --     Tamarack Consulting
   --
   --     all rights reserved.
   --  THIS WORK MAY NOT BE COPIED FOR ANY REASON
   --  WITHOUT EXPRESSED WRITTEN PERMISSION OF AN
   --  OFFICER OF Tamarack Consulting.
   --
   ------------------------------------------------------

*/

#ifndef _IGO_TAB_H
#define _IGO_TAB_H
typedef union{                          /* The attribute stack declaration         */
    int     _length;             /* Sub-Message Lengths                     */
    int     _value;              /* Passing values up the translation stack */
    MMSd_InvokeIdType _ID;       /* For passing Invoke IDs up the stack     */
    MMSd_descriptor  TKN;        /* Non-decoded Terminal Symbols            */
} YYSTYPE;
extern YYSTYPE igolval;
#endif /* _IGO_TAB_H */
/* end go_tab.h */
