/*
 * Excerpted from "Test-Driven Development for Embedded C",
 * published by The Pragmatic Bookshelf.
 * Copyrights apply to this code. It may not be used to create 
 * training material, courses, books, articles, and the like. 
 * Contact us if you are in doubt. We make no guarantees that 
 * this code is fit for any purpose. Visit to^
 * http://www.pragmaticprogrammer.com/titles/jgade for more book information.
 *
 *     Copyright (c) James W. Grenning -- All Rights Reserved           *
 *     For use by owners of Test-Driven Development for Embedded C,     *
 *     and attendees of Renaissance Software Consulting, Co. training   *
 *     classes.                                                         *
 *                                                                      *
 *     Available at http://pragprog.com/titles/jgade/                   *
 *         ISBN 1-934356-62-X, ISBN13 978-1-934356-62-3                 *
 *                                                                      *
 *     Authorized users may use this source code in your own            *
 *     projects, however the source code may not be used to             *
 *     create training material, courses, books, articles, and          *
 *     the like. We make no guarantees that this source code is         *
 *     fit for any purpose.                                             *
 *                                                                      *
 *     www.renaissancesoftware.net james@renaissancesoftware.net        *
 *                                                                      */

#include <stdint.h>
#include "CircularBuffer.h"

/**
 * @brief  Structure describing ring buffer
 */
typedef struct CircularBufferStruct                                             // Структура описывающая кольцевой буфер
{                                                                               
    uint16_t count;                                                             // Счётчик занятых элементов буфера
    uint16_t index;                                                             // Индекс поступившего элемента
    uint16_t outdex;                                                            // Индекс извлечённого элемента
    uint16_t capacity;                                                          // Глубина буфера
    uint32_t * values;                                                          // Буфер заданной глубины
} CircularBufferStruct ;                                                        

enum {BUFFER_GUARD = 999};                                                      // Признак конца кольцевого буфера.

/**
 * @brief   Create a ring buffer
 * @param   capacity: ring buffer capacity
 * @retval  self:     pointer to the ring buffer structure
 */
CircularBuffer CircularBuffer_Create(uint16_t capacity)                         // Функция создания кольцевого буфера заданной глубины
{
    CircularBuffer self = calloc(capacity, sizeof(CircularBufferStruct));       // Создание буфера путём выделения памяти с возвратом указателя на неё
    if (self == NULL)                                                           // Когда выделение памяти прошло неудачно ->
    {
        CircularBuffer_Destroy(self);                                           // Уничтожение выделенной памяти
        return NULL;                                                            // Возврат ни с чем
    }
    self->capacity = capacity;                                                  // Сохранение запрошенной глубины
    self->values = calloc(capacity + 1, sizeof(uint32_t));                      // Выделение запрошенной памяти 
    if (self == NULL)                                                           // Когда выделение памяти прошло неудачно ->
    {
        CircularBuffer_Destroy(self);                                           // Уничтожение выделенной памяти
        return NULL;                                                            // Возврат ни с чем
    }

    self->values[capacity] = BUFFER_GUARD;                                      // В последнюю ячейку выделенной памяти кладётся признак конца буфера
    return self;                                                                // Возврат указателя на созданный буфер
}

/**
 * @brief   Destroy a ring buffer
 * @param   self:     pointer to the ring buffer structure
 * @retval  None
 */
void CircularBuffer_Destroy(CircularBuffer self)                                // Функция уничтожения кольцевого буфера
{
    if(self->values != NULL)                                                    // Когда буфер не пуст ->
    {
        free(self->values);                                                     // Освобождение занятой памяти
    }
    if(self != NULL)                                                            // Когда буфер существует ->
    {
        free(self);                                                             // Удаление буфера из памяти
    }
}

/**
 * @brief   Check integrity of the ring buffer
 * @param   self:  pointer to the ring buffer structure
 * @retval  bool:  true | false
 */
bool CircularBuffer_VerifyIntegrity(CircularBuffer self)                        // Функция проверки целостности кольцевого буфера
{
	return self->values[self->capacity] == BUFFER_GUARD;                        // Сверка признака конца кольцевого буфера через указатель на последний элемент. Возврат TRUE, если буфер в порядке. 
}

/**
 * @brief   Check for an empty of the ring buffer
 * @param   self:  pointer to the ring buffer structure
 * @retval  bool:  true | false
 */
bool CircularBuffer_IsEmpty(CircularBuffer self)                                // Функция определения пустого буфера
{
    return self->count == 0;                                                    // Сверка количества заполненных ячеек буфера с 0. Возврат TRUE, если буфер пуст. 
}

/**
 * @brief   Check for a full of the ring buffer
 * @param   self:  pointer to the ring buffer structure
 * @retval  bool:  true | false
 */
bool CircularBuffer_IsFull(CircularBuffer self)                                 // Функция определения заполненного буфера
{
    return self->count == self->capacity;                                       // Сверка количества заполненных ячеек буфера с его глубиной. Возврат TRUE, если буфер заполнен. 
}

/**
 * @brief   Extract the value of the ring buffer
 * @param   self:    pointer to the ring buffer structure
 *          pValue:  extracted value
 * @retval  status:  OK
 *                   OUT_OF_BOUNDS
 */
Status CircularBuffer_Put(CircularBuffer self, uint32_t value)                  // Функция заталкивания значения в буфер
{
    if (self->count >= self->capacity)                                          // Когда буфер полностью заполнен ->
    {
        return OUT_OF_BOUNDS;                                                   // Возврат OUT_OF_BOUNDS
    }

    self->count++;                                                              // Инкремент счётчика элементов буфера
    self->values[self->index++] = value;                                        // Укладка поступившего значения в буфер
    if (self->index >= self->capacity)                                          // Когда индекс поступившего элемента буфера больше его глубины ->
    {
        self->index = 0;                                                        // Индекс поступившего элемента буфера = 0
    }
    return OK;                                                                  // Возврат OK
}

/**
 * @brief   Retrieves the value of the ring buffer
 * @param   self:    pointer to the ring buffer structure
 *          pValue:  extracted value
 * @retval  status:  OK
 *                   NO_DATA
 */
Status CircularBuffer_Get(CircularBuffer self, uint32_t *pValue)                // Функция выталкивания значения из буфера
{
    if (self->count <= 0)                                                       // Когда буфер пуст ->
    {
        return NO_DATA;                                                         // Возврат NO_DATA
    }

    *pValue = self->values[self->outdex++];                                     // Извлечение значения из буфера
    self->count--;                                                              // Декремент счётчика элементов буфера
    if (self->outdex >= self->capacity)                                         // Когда индекс извлеченного элемента из буфера больше его глубины ->
    {
        self->outdex = 0;                                                       // Индекс извлеченного элемента из буфера = 0
    }
    return OK;                                                                  // Возврат OK
}

/**
 * @brief   Return the capacity of the ring buffer
 * @param   self:     pointer to the ring buffer structure
 * @retval  uint16_t: ring buffer capacity
 */
uint16_t CircularBuffer_Capacity(CircularBuffer self)                           // Функция возврата глубины кольцевого буфера
{
    return self->capacity;                                                      // Возврат глубины кольцевого буфера
}
