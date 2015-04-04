/* Based upon John Levine's book "flex&bison" */
#ifndef _SUBEXPRESSION_H
#define _SUBEXPRESSION_H

typedef enum
{
    typeInt,         /* Целое */
    typeString,      /* Строковое */
    typeError,       /* ошибка */
    typeFloat				 /* с плавающей точкой */
} BaseTypeEnum;

#endif
