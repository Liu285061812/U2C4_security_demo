/******************************************************************************/
/* Component Name   ISSD (ICU-M Start-up & Service Dispatcher)                */
/******************************************************************************/
/* Product      : ICU-M Low Level Driver (Demo Part)                          */
/******************************************************************************/
/***********************************************************************************************************************
* RENESAS ELECTRONICS CONFIDENTIAL AND PROPRIETARY
* 
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* www.renesas.com/disclaimer
*
* Copyright (C) 2021-2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/* Header files                     */
#include "renesas_types.h"
#include "r_icumif_api.h"
#include "icum_common_pub.h"
#include "icum_d_misc_pub.h"
#include "icum_l_toolbox_pub.h"

#include "icum_d_comm_pub.h"
#include "icum_d_int_pub.h"
#include "icum_k_issd_pub.h"
#include "icum_k_issd.h"

/***************************************************************************************
* Local Definition                                                                     *
***************************************************************************************/
/* Number of queue type */
#define ISSD_NUM_OF_QUEUE_TYPE   (4)
/* Queue type */
#define ISSD_QUEUE_TYPE_SPECIAL  (0)
#define ISSD_QUEUE_TYPE_HIGH     (1)
#define ISSD_QUEUE_TYPE_MIDDLE   (2)
#define ISSD_QUEUE_TYPE_LOW      (3)
#define ISSD_QUEUE_TYPE_NONE  (0xFFu)
/* Execution frequency ratio */
#define ISSD_EXEC_RATIO_SPECIAL   (8UL)
#define ISSD_EXEC_RATIO_HIGH      (4UL)
#define ISSD_EXEC_RATIO_MIDDLE    (2UL)
#define ISSD_EXEC_RATIO_LOW       (1UL)
/* Maximum job execution counter */
#define ISSD_MAX_JOB_EXEC_COUNTER   (0xFFFFFFFFUL)

#define ISSD_LIST_END       (r_icumif_service_id_t)EXPANSION_SERVICE_BIT

/***************************************************************************************
* Global Variable                                                                      *
***************************************************************************************/
/* The job table */
static icum_job_t g_job_table[ISSD_MAX_NB_OF_JOBS];

/* The job queue */
static icum_job_queue_t g_job_queue[ISSD_NUM_OF_QUEUE_TYPE][ISSD_MAX_NB_OF_JOBS];

/* The indexes on the job table */
/* NOTE! The indexes *MUST* be signed */
static int8_t g_first_job_idx[ISSD_NUM_OF_QUEUE_TYPE];
static int8_t g_last_job_idx[ISSD_NUM_OF_QUEUE_TYPE];
static int8_t g_current_job_idx[ISSD_NUM_OF_QUEUE_TYPE];
static int8_t g_new_job_idx[ISSD_NUM_OF_QUEUE_TYPE];
static uint8_t g_current_job_counter[ISSD_NUM_OF_QUEUE_TYPE];
static uint32_t g_job_exec_counter[ISSD_NUM_OF_QUEUE_TYPE];

/* The job ID counter */
static uint16_t g_job_id_counter;

/* count of current all JOB */
static uint8_t g_current_all_job_counter;

/* Current queue index */
static int8_t g_current_queue_type;

static uint32_t         g_requested_gen_counter;
static r_icumif_isd_t   *g_generate_ISD[MAX_REGIST_SRV_GEN_REQ];

/* Can not use services from exp service */
static const r_icumif_service_id_t g_disable_service[2] =
{
    SERVICE_00_ECC_TEST,
    /* end of table */
    ISSD_LIST_END
};

/* Execution frequency ratio table */
CHANGE_SEC(rodata, ".icum_kernel_const_ram")
static uint32_t g_exec_ratio[4] =
{
    ISSD_EXEC_RATIO_SPECIAL,
    ISSD_EXEC_RATIO_HIGH,
    ISSD_EXEC_RATIO_MIDDLE,
    ISSD_EXEC_RATIO_LOW
};

#pragma ghs reference start_internal_service
CHANGE_SEC(rodata, default)
/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0001]:[ICUMFW_UD_K_ISSD_0001]
* Function name: k_ISSD_InitJobQueue
* Description  : Initialization of the job queue
* Arguments    : none
* Return Value : none
***************************************************************************************/
void k_ISSD_InitJobQueue(void)
{
    int8_t i;
    int8_t j;

    /* init indexes in job queue */
    for(i = 0; i < ISSD_NUM_OF_QUEUE_TYPE; i++)
    {
        g_first_job_idx[i]   = -1;
        g_last_job_idx[i]    = -1;
        g_current_job_idx[i] = -1;
        g_new_job_idx[i]     = -1;
        g_current_job_counter[i] = 0u;
        g_job_exec_counter[i]    = 0uL;

        /* clear job queue */
        for(j = 0; j < ISSD_MAX_NB_OF_JOBS; j++)
        {
            g_job_queue[i][j].next_job_idx = 0x00u;
            g_job_queue[i][j].prev_job_idx = 0x00u;
            g_job_queue[i][j].table_idx    = ISSD_QUEUE_EMPTY;
        }
    }

    /* clear job table */
    for(i = 0; i < ISSD_MAX_NB_OF_JOBS; i++)
    {
        L_Toolbox_MemSet((uint8_t *)&g_job_table[i], (uint8_t)0, (int16_t)sizeof(icum_job_t));
    }

    /* first job ID must be greater than 0!! */
    g_job_id_counter  = 1u;
    /* clear current queue type */
    g_current_queue_type = -1;
    /* clear number of job */
    g_current_all_job_counter = 0u;

}

/*******************************************************************************/
/* make the below functions available when operating from RAM                  */
/*******************************************************************************/
CHANGE_SEC(text, ".icum_kernel_ram")
CHANGE_SEC(rodata, ".icum_kernel_const_ram")

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0012]:[ICUMFW_UD_K_ISSD_0012]
* Function name: k_ISSD_GetQueueType
* Description  : get queue index from service priority
* Arguments    : (IN)  service priority
*                (OUT) queue index
* Return Value : none
***************************************************************************************/
void k_ISSD_GetQueueType(r_icumif_service_priority_t service_priority, uint8_t *p_queue_type)
{
    uint8_t queue_type;

    if((uint32_t)R_ICUMIF_SERV_PRIORITY_HIGHEST == (uint32_t)service_priority)
    {
        queue_type = (uint8_t)ISSD_QUEUE_TYPE_SPECIAL;
    }
    else if((uint32_t)R_ICUMIF_SERV_PRIORITY_HIGH >= (uint32_t)service_priority)
    {
        queue_type = (uint8_t)ISSD_QUEUE_TYPE_HIGH;
    }
    else if((uint32_t)R_ICUMIF_SERV_PRIORITY_NORMAL >= (uint32_t)service_priority)
    {
        queue_type = (uint8_t)ISSD_QUEUE_TYPE_MIDDLE;
    }
    else /*if(R_ICUMIF_SERV_PRIORITY_LOWEST >= service_priority)*/
    {
        queue_type = (uint8_t)ISSD_QUEUE_TYPE_LOW;
    }
    *p_queue_type = queue_type;
    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0004]:[ICUMFW_UD_K_ISSD_0004]
* Function name: k_ISSD_AddJobQueue
* Description  : Adding a job in the job queue
* Arguments    : (IN)  *p_ISD :
*                (IN)  *p_ghost_job :
*                (OUT) *p_job_id :
*                      0     : if the job queue is full.
*                      other : the job counter (always above 0!!)
* Return Value : none
***************************************************************************************/
void k_ISSD_AddJobQueue(r_icumif_isd_t *p_ISD, icum_job_t *p_ghost_job, r_icumif_job_id_t *p_job_id)
{
    uint32_t saved_psw;
    int8_t i;
    int8_t j;
    r_icumif_job_id_t job_id = 0u;
    uint8_t que_type;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* check JOB counter */
    if((uint32_t)ISSD_MAX_NB_OF_JOBS <= (uint32_t)g_current_all_job_counter)
    {
        /* Disable interrupt because the job queue full */
        (void)D_COMM_SetInterruptSwitch(ICUM_COMM_ICH_FM_SERV_REQUEST, ICUM_COMM_INT_REQ_DISABLE);
    }
    else
    {
        /* get queue index from service priority */
        k_ISSD_GetQueueType(p_ISD->service_priority, &que_type);

        /* find first available slot in table */
        i = -1;
        for(j = 0; j < ISSD_MAX_NB_OF_JOBS; j++)
        {
            if((NULL == g_job_table[j].p_ISD) &&
            (0UL == (uint32_t)g_job_table[j].keep.job_ghost) &&
            (0UL == (uint32_t)g_job_table[j].keep.job_cycle))
            {
                /* found! */
                i = j;
                break;
            }
            else
            {
                /* Do nothing */
            }
        }

        if(0 > i)
        {
            /* no slot available: try again without testing keep.job_ghost */
            /* (doing some garbage collection) */
            for(j = 0; j < ISSD_MAX_NB_OF_JOBS; j++)
            {
                if(NULL == g_job_table[j].p_ISD)
                {
                    if((p_ghost_job != &g_job_table[j]) &&
                    (0UL == (uint32_t)g_job_table[j].keep.job_cycle))
                    {
                        /* found! */
                        i = j;
                        break;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
        }
        else
        {
            /* Do nothing */
        }

        if(0 <= i)
        {
            /* copy parameters from the ghost job if available */
            if(NULL != p_ghost_job)
            {
                /* note: sizeof() not called (resolved at compile time), thus no issue when running from RAM */
                L_Toolbox_MemCpy((uint8_t *)&g_job_table[i], (const uint8_t *)p_ghost_job,
                                (int16_t)sizeof(icum_job_t));
            }
            else
            {
                /* note: sizeof() not called (resolved at compile time), thus no issue when running from RAM */
                L_Toolbox_MemSet((uint8_t *)&g_job_table[i], (uint8_t)0, (int16_t)sizeof(icum_job_t));
            }

            /* init job in slot */
            g_job_table[i].p_ISD = p_ISD;

            /* save requester id */
            g_job_table[i].requester_id = p_ISD->requester_id;
            /* save vm id */
            g_job_table[i].vm_id = p_ISD->vm_id;
            /* save priority */
            g_job_table[i].priority = (uint8_t)(p_ISD->service_priority);
            /* by default, do not keep the job ghost */
            g_job_table[i].keep.job_ghost = 0U;
            g_job_table[i].keep.job_cycle = 0U;

            /* search empty table index */
            for(j = 0; j < ISSD_MAX_NB_OF_JOBS; j++)
            {
                if(g_job_queue[que_type][j].table_idx == ISSD_QUEUE_EMPTY)
                {
                    g_job_queue[que_type][j].table_idx = (uint8_t)i;
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }


            if(0 > g_first_job_idx[que_type])
            {
                /* job queue empty */
                /* add in first table position */
                g_current_job_idx[que_type]            = j;
                g_first_job_idx[que_type]              = j;
                g_last_job_idx[que_type]               = j;
                g_new_job_idx[que_type]                = j;
                g_job_queue[que_type][j].prev_job_idx  = (uint8_t)j;
                g_job_queue[que_type][j].next_job_idx  = (uint8_t)j;
            }
            else
            {
                /* job inserted after last job in queue */
                /* note: could have more options here!!! */
                g_job_queue[que_type][j].next_job_idx = g_job_queue[que_type][g_last_job_idx[que_type]].next_job_idx;
                g_job_queue[que_type][j].prev_job_idx = (uint8_t)g_last_job_idx[que_type];

                g_job_queue[que_type][g_first_job_idx[que_type]].prev_job_idx = (uint8_t)j;
                g_job_queue[que_type][g_last_job_idx[que_type]].next_job_idx  = (uint8_t)j;
                g_last_job_idx[que_type] = j;
            }
            /* set new job id */
            g_job_table[i].job_id  = g_job_id_counter;

            /* return the job id counter */
            job_id = g_job_id_counter;

            /* increment job counter */
            g_job_id_counter++;
            if((uint32_t)0 == (uint32_t)g_job_id_counter)
            {
                g_job_id_counter = 1u;
            }
            else
            {
                /* Do nothing */
            }
            /* increment number of job */
            g_current_job_counter[que_type]++;
            g_current_all_job_counter++;
        }
        else
        {
            /* Do nothing */
        }
    }

    *p_job_id = job_id;
    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return;
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0002]:[ICUMFW_UD_K_ISSD_0002]
* Function name: start_internal_service
* Description  : adds an internal service to the job queue
*                returns with id of the job created (0 if not successfull i.e. job queue full)
* Arguments    : (IN) service_id
*                (IN/OUT) *p_service
*                (IN) from_service_id
* Return Value : SERV_OK                : Success.
*                SERV_PRM_ERROR         : Parameter error
***************************************************************************************/
r_icumif_service_result_t start_internal_service(r_icumif_service_id_t service_id, r_icumif_isd_t *p_service, r_icumif_service_id_t from_service_id)
{
    r_icumif_job_id_t   new_job_id;
    r_icumif_service_result_t rtn = SERV_OK;
    uint32_t                  count = 0u;

    /* check starting service */
    while(g_disable_service[count] != ISSD_LIST_END)
    {
        if(g_disable_service[count] == service_id)
        {
            /* can not start */
            rtn = SERV_PRM_ERROR;
            break;
        }
        else
        {
            /* Do nothing */
        }
        count++;
    }

    if (SERV_OK == rtn)
    {
        /* 1. set parameter */
        p_service->service_id     = service_id;
        p_service->requester_id   = (r_icumif_requester_id_t)GET_MY_PEID;
        p_service->p_callbackfunc = NULL;
        p_service->service_result = SERV_REQUEST;
        p_service->job_id         = 0u;

        /* 2. add service */
        (void)K_ISSD_AddService(p_service, &new_job_id);
        p_service->job_id = new_job_id;

        /* Judge request from expansion service */
        if(EXPANSION_SERVICE_BIT == (from_service_id & EXPANSION_SERVICE_BIT))
        {
            (void)k_ISSD_SetExpansionService(new_job_id, FROM_EXPANSION_SERVICE);
        }
        else
        {
            (void)k_ISSD_SetExpansionService(new_job_id, FROM_NATIVE_SERVICE);
        }
    }
    else
    {
        /* Do nothing */
    }

    return (rtn);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0003]:[ICUMFW_UD_K_ISSD_0003]
* Function name: K_ISSD_AddService
* Description  : Adding a job in the job queue
* Arguments    : (IN)  *p_ISD :
*                (OUT) *p_job_id :
*                      0     : if the job queue is full.
*                      other : the job counter (always above 0!!)
* Return Value : ER_OK    : success
*                ER_PARAM : if the job queue is full.
***************************************************************************************/
uint32_t K_ISSD_AddService(r_icumif_isd_t *p_isd, r_icumif_job_id_t *p_job_id)
{
    uint32_t rtn = ER_OK;

    k_ISSD_AddJobQueue(p_isd, NULL, p_job_id);
    if ((uint32_t)0 == (uint32_t)*p_job_id)
    {
        rtn = ER_PARAM;
    }
    else
    {
        /* Do nothing */
    }

    return(rtn);
}

CHANGE_SEC(text, ".icum_kernel_ram")
CHANGE_SEC(rodata, ".icum_kernel_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0013]:[ICUMFW_UD_K_ISSD_0013]
* Function name: k_ISSD_ClearJobExecCounter
* Description  : clear the counter of job executed
* Arguments    : none
* Return Value : none
***************************************************************************************/
void k_ISSD_ClearJobExecCounter(void)
{
    int8_t i;

    /* init indexes in job queue */
    for(i = 0; i < ISSD_NUM_OF_QUEUE_TYPE; i++)
    {
        g_job_exec_counter[i] = 0uL;
    }
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0005]:[ICUMFW_UD_K_ISSD_0005]
* Function name: k_ISSD_RemoveCurrentJob
* Description  : Removing the current job from the queue
* Arguments    : none
* Return Value : none
***************************************************************************************/
void k_ISSD_RemoveCurrentJob(void)
{
    uint32_t saved_psw;
    int8_t  table_idx;
    int8_t  queue_idx;
    int8_t  que_type;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    que_type = g_current_queue_type;
    if(0 > que_type)
    {
        /* job queue empty */
        table_idx = -1;
    }
    else
    {
        queue_idx = g_current_job_idx[que_type];
        table_idx = g_job_queue[que_type][queue_idx].table_idx;
    }

    /* job queue empty? */
    if (0 <= table_idx)
    {
        /* at this point i = index of the job to remove */
        /* clear the pointer to the ISD structure */
        g_job_table[table_idx].p_ISD = NULL;

        /* clear the requester id */
        g_job_table[table_idx].requester_id = 0u;

        /* clear table index */
        g_job_queue[que_type][queue_idx].table_idx = ISSD_QUEUE_EMPTY;

        if(g_first_job_idx[que_type] == g_last_job_idx[que_type])
        {
            /* only one job remaining in the queue */
            /* now job queue empty */
            g_first_job_idx[que_type]   = -1;
            g_last_job_idx[que_type]    = -1;
            g_current_job_idx[que_type] = -1;
            g_new_job_idx[que_type]     = -1;
        }
        else
        {
            /* two or more job remaining in the queue */
            g_new_job_idx[que_type] = (int8_t)g_job_queue[que_type][queue_idx].next_job_idx;
            g_job_queue[que_type][g_job_queue[que_type][queue_idx].prev_job_idx].next_job_idx = (uint8_t)g_new_job_idx[que_type];
            g_job_queue[que_type][g_new_job_idx[que_type]].prev_job_idx = g_job_queue[que_type][queue_idx].prev_job_idx;
            if(g_first_job_idx[que_type] == queue_idx)
            {
                g_first_job_idx[que_type] = (int8_t)g_job_queue[que_type][queue_idx].next_job_idx;
            }
            else
            {
                /* Do nothing */
            }
            if(g_last_job_idx[que_type] == queue_idx)
            {
                g_last_job_idx[que_type] = (int8_t)g_job_queue[que_type][queue_idx].prev_job_idx;
            }
            else
            {
                /* Do nothing */
            }
        }

        /* decrement number of job */
        g_current_job_counter[que_type]--;
        g_current_all_job_counter--;
        if((uint32_t)0 == (uint32_t)g_current_all_job_counter)
        {
            /* clear the counter of job executed */
            k_ISSD_ClearJobExecCounter();
        }
        else
        {
            if((uint32_t)(ISSD_MAX_NB_OF_JOBS - 1) == (uint32_t)g_current_all_job_counter)
            {
                /* Enable interrupt because the job queue empty */
                (void)D_COMM_SetInterruptSwitch(ICUM_COMM_ICH_FM_SERV_REQUEST, ICUM_COMM_INT_REQ_ENABLE);
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    else
    {
        /* Do nothing */
    }

    /* 5. re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0014]:[ICUMFW_UD_K_ISSD_0014]
* Function name: k_ISSD_JudgePriority
* Description  : Determine the priority and get the type of queue to get next
* Arguments    : (OUT) *p_next_que_type : Next queue type to be acquired
* Return Value : none
***************************************************************************************/
void k_ISSD_JudgePriority(int8_t *p_next_que_type)
{
    uint8_t next_que_type;
    int8_t i;
    int8_t j;
    uint32_t exec_ratio;

    next_que_type = ISSD_QUEUE_TYPE_NONE;

    /* Only when the last execution is higher than LOW, */
    /* the number of times of distribution is judged.    */
    if(ISSD_QUEUE_TYPE_LOW != g_current_queue_type)
    {
        /* loop for special priority to middle priority */
        for(i = g_current_queue_type; i < (ISSD_NUM_OF_QUEUE_TYPE - 1); i++)
        {
            /* loop for next priority to low priority */
            for(j = (i + 1); j < ISSD_NUM_OF_QUEUE_TYPE; j++)
            {
                /* Low priority JOB exist & exec counter high > low */
                if((uint32_t)0 < (uint32_t)g_current_job_counter[j])
                {
                    /* Calculation of execution ratio */
                    exec_ratio = g_exec_ratio[i] / g_exec_ratio[j];
                    if((g_job_exec_counter[i] / (g_job_exec_counter[j] + 1uL)) >= exec_ratio)
                    {
                        next_que_type = (uint8_t)j;
                        break;
                    }
                    else
                    {
                        /* Do nothing */
                    }
                }
                else
                {
                    /* Do nothing */
                }
            }
            if((uint32_t)ISSD_QUEUE_TYPE_NONE != (uint32_t)next_que_type)
            {
                break;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    else
    {
        /* Do nothing */
    }
    /* no low priority JOB to be executed */
    if((uint32_t)ISSD_QUEUE_TYPE_NONE == (uint32_t)next_que_type)
    {
        /* search of higest priority JOB */
        for(i = 0; i < ISSD_NUM_OF_QUEUE_TYPE; i++)
        {
            if((uint32_t)0 < (uint32_t)g_current_job_counter[i])
            {
                next_que_type = (uint8_t)i;
                break;
            }
            else
            {
                /* Do nothing */
            }
        }
    }
    else
    {
        /* Do nothing */
    }
    *p_next_que_type = (int8_t)next_que_type;
    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0006]:[ICUMFW_UD_K_ISSD_0006]
* Function name: k_ISSD_GetNextJob
* Description  : Retrieve next job in job queue
* Arguments    : (OUT) **p_next_job : next job table address
*                      0     : if the job queue is empty.
*                      other : a pointer to the job to process.
* Return Value : none
***************************************************************************************/
void k_ISSD_GetNextJob(icum_job_t **p_next_job)
{
    uint32_t saved_psw;
    icum_job_t * p_job;
    int8_t que_type;
    int8_t i;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    if((uint32_t)0 < (uint32_t)g_current_all_job_counter)
    {
        /* judge the priority */
        k_ISSD_JudgePriority(&que_type);

        /* check exist job */
        if(0 <= g_current_job_idx[que_type])
        {
            if(g_current_job_idx[que_type] != g_new_job_idx[que_type])
            {
                 g_current_job_idx[que_type] = g_new_job_idx[que_type];
            }
            else
            {
                /* Do nothing */
            }
            i = g_job_queue[que_type][g_current_job_idx[que_type]].table_idx;
            p_job = &g_job_table[i];
            g_new_job_idx[que_type] = (int8_t)g_job_queue[que_type][g_current_job_idx[que_type]].next_job_idx;

            /* update job execute counter */
            g_job_exec_counter[que_type]++;
            /* execute counter over flow */
            if(ISSD_MAX_JOB_EXEC_COUNTER == g_job_exec_counter[que_type])
            {
                /* clear all JOB execute counter */
                k_ISSD_ClearJobExecCounter();
            }
            else
            {
                /* Do nothing */
            }
            /* set current queue type */
            g_current_queue_type = que_type;
        }
        else
        {
            /* job queue empty */
            p_job = NULL;
        }
    }
    else
    {
        /* job queue empty */
        p_job = NULL;
    }
    *p_next_job = p_job;

    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0007]:[ICUMFW_UD_K_ISSD_0007]
* Function name: k_ISSD_GetJob
* Description  : Retrieve a specific job in queue based on its job id
* Arguments    : (IN)  job_id :
*                (OUT) **p_specif_job : job table address of a specified job
*                      -1    : if the job id specified is not found in the queue.
*                      other : a pointer to the job to process.
* Return Value : none
***************************************************************************************/
void k_ISSD_GetJob(r_icumif_job_id_t job_id, icum_job_t **p_specif_job)
{
    uint32_t saved_psw;
    icum_job_t * p_job;
    int8_t i;
    int8_t index;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* index initialize */
    index = -1;
    /* search for the job_id in the job table (not queue!) */
    for(i = 0; i < ISSD_MAX_NB_OF_JOBS; i++)
    {
        if((uint32_t)g_job_table[i].job_id == (uint32_t)job_id)
        {
            /* found! */
            index = i;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    /* not found */
    if(0 > index)
    {
        /* job not found: signal the error */
        p_job = NULL;
    }
    else
    {
        /* job found: return the pointer to it */
        p_job = &g_job_table[index];
        /* make sure this table slot can be used again! */
        p_job->keep.job_ghost = 0U;
        p_job->keep.job_cycle = 0U;
    }
    *p_specif_job = p_job;

    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0008]:[ICUMFW_UD_K_ISSD_0008]
* Function name: k_ISSD_GetAllJobCount
* Description  : Counts (and returns) the number of jobs currently in the queue
* Arguments    : (OUT) *p_count : the number of jobs.
* Return Value : ER_OK
***************************************************************************************/
uint32_t k_ISSD_GetAllJobCount(uint32_t *p_count)
{
    uint32_t saved_psw;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    *p_count = (uint32_t)g_current_all_job_counter;

    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return(ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0009]:[ICUMFW_UD_K_ISSD_0009]
* Function name: K_ISSD_GetJobCount
* Description  : Counts (and returns) the number of jobs currently in the queue, cycle and ghost
* Arguments    : (OUT) *p_count : the number of jobs(cycle & ghost).
* Return Value : ER_OK
***************************************************************************************/
uint32_t K_ISSD_GetJobCount(uint32_t *p_count)
{
    uint32_t saved_psw;
    int8_t i;
    uint8_t job_count = 0u;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* search for the job_id in the job table (not queue!) */
    for(i = 0; i < ISSD_MAX_NB_OF_JOBS; i++)
    {
        if((NULL == g_job_table[i].p_ISD) &&
           ((0UL != (uint32_t)g_job_table[i].keep.job_ghost) ||
            (0UL != (uint32_t)g_job_table[i].keep.job_cycle)))
        {
            /* found! */
            job_count++;
        }
        else
        {
            /* Do nothing */
        }
    }

    *p_count = (uint32_t)g_current_all_job_counter + (uint32_t)job_count;

    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return(ER_OK);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0010]:[ICUMFW_UD_K_ISSD_0010]
* Function name: K_ISSD_GetServiceID
* Description  : Get service id for job id
* Arguments    : (IN)  job_id :
*              : (OUT) p_service_id
* Return Value : ER_OK : success
*                ER_K_ISSD_JOB_NOT_FOUND : if the job id specified is not found in the queue.
***************************************************************************************/
uint32_t K_ISSD_GetServiceID(r_icumif_job_id_t job_id, r_icumif_service_id_t *p_service_id)
{
    uint32_t result;
    uint32_t saved_psw;
    int8_t i;
    int8_t index;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* index initialize */
    index = -1;
    /* search for the job_id in the job table (not queue!) */
    for(i = 0; i < ISSD_MAX_NB_OF_JOBS; i++)
    {
        if((NULL != g_job_table[i].p_ISD) &&
           ((uint32_t)g_job_table[i].job_id == (uint32_t)job_id))
        {
            /* found! */
            index = i;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    /* not found */
    if(0 > index)
    {
        /* job not found: signal the error */
        result = ER_K_ISSD_JOB_NOT_FOUND;
    }
    else
    {
        /* 4. job found: get service id */
        *p_service_id = g_job_table[i].p_ISD->service_id;
        result = ER_OK;
    }

    /* 5. re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return(result);
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0011]:[ICUMFW_UD_K_ISSD_0011]
* Function name: K_ISSD_SetCancelRequest
* Description  : Set cancel request for job
* Arguments    : (IN)  job_id :
* Return Value : ER_OK : success
*                ER_K_ISSD_JOB_NOT_FOUND : if the job id specified is not found in the queue.
***************************************************************************************/
uint32_t K_ISSD_SetCancelRequest(r_icumif_job_id_t cancel_job_id)
{
    uint32_t result;
    uint32_t saved_psw;
    int8_t i;
    int8_t index;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* index initialize */
    index = -1;
    /* search for the job_id in the job table (not queue!) */
    for(i = 0; i < ISSD_MAX_NB_OF_JOBS; i++)
    {
        if((NULL != g_job_table[i].p_ISD) &&
           ((uint32_t)g_job_table[i].job_id == (uint32_t)cancel_job_id))
        {
            /* found! */
            index = i;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    /* not found */
    if(0 > index)
    {
        /* job not found: signal the error */
        result = ER_K_ISSD_JOB_NOT_FOUND;
    }
    else
    {
        /* job found: get service id */
        g_job_table[i].cancel_request = JOB_CANCEL_REQUESTED;
        result = ER_OK;
    }

    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return(result);
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0015]:[ICUMFW_UD_K_ISSD_0015]
* Function name: k_ISSD_SetExpansionService
* Description  : Set expansion service for job
* Arguments    : (IN)  job_id :
* Return Value : ER_OK : success
*                ER_K_ISSD_JOB_NOT_FOUND : if the job id specified is not found in the queue.
***************************************************************************************/
uint32_t k_ISSD_SetExpansionService(r_icumif_job_id_t job_id, uint8_t set_value)
{
    uint32_t result;
    uint32_t saved_psw;
    int8_t i;
    int8_t index;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    /* index initialize */
    index = -1;
    /* search for the job_id in the job table (not queue!) */
    for(i = 0; i < ISSD_MAX_NB_OF_JOBS; i++)
    {
        if((NULL != g_job_table[i].p_ISD) &&
           ((uint32_t)g_job_table[i].job_id == (uint32_t)job_id))
        {
            /* found! */
            index = i;
            break;
        }
        else
        {
            /* Do nothing */
        }
    }

    /* not found */
    if(0 > index)
    {
        /* job not found: signal the error */
        result = ER_K_ISSD_JOB_NOT_FOUND;
    }
    else
    {
        /* job found: get service id */
        g_job_table[i].expansion_service = set_value;
        result = ER_OK;
    }

    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return(result);
}


/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0016]:[ICUMFW_UD_K_ISSD_0016]
* Function name: k_ISSD_InitGenService
* Description  : Initialize of generate service request
* Arguments    : none
* Return Value : ER_OK
***************************************************************************************/
uint32_t k_ISSD_InitGenService(void)
{
    uint32_t i;

    /* Initialization of  service generation request counter */
    g_requested_gen_counter = 0UL;
    /* Initialization of  service generation request pointer */
    for(i = 0UL; i < MAX_REGIST_SRV_GEN_REQ; i++)
    {
        g_generate_ISD[i] = NULL;
    }
    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0017]:[ICUMFW_UD_K_ISSD_0017]
* Function name: k_ISSD_IsRegistGenService
* Description  : check for registered of service generation request
* Arguments    : (OUT) p_registered
*                true  : registered
*                false : no registered
* Return Value : ER_OK
***************************************************************************************/
uint32_t k_ISSD_IsRegistGenService(bool *p_registered)
{
    uint32_t saved_psw;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    if(0UL < g_requested_gen_counter)
    {
        *p_registered = (bool)true;
    }
    else
    {
        *p_registered = (bool)false;
    }
    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return ER_OK;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0018]:[ICUMFW_UD_K_ISSD_0018]
* Function name: K_ISSD_GenerateServiceRequest
* Description  : Generate service request
* Arguments    : (IN)  *p_ISD : pointer of ISD
* Return Value : ER_OK    : success
*                ER_PARAM : if the NULL pointer.
*                ER_K_ISSD_REGIST_FULL : if the regist request FULL.
***************************************************************************************/
uint32_t K_ISSD_GenerateServiceRequest(r_icumif_isd_t *p_isd)
{
    uint32_t i;
    uint32_t result = ER_OK;
    uint32_t saved_psw;

    if(NULL == p_isd)
    {
        result = ER_PARAM;
    }
    else
    {
        /* disable interrupts (if not already disabled) */
        DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

        for(i = 0UL; i < MAX_REGIST_SRV_GEN_REQ; i++)
        {
            if(NULL == g_generate_ISD[i])
            {
                /* saved of service generation ISD pointer */
                g_generate_ISD[i] = p_isd;
                /* incremented of service generation request counter */
                g_requested_gen_counter++;
                break;
            }
            else
            {
                /* do nothing */
            }
        }
        /* re-enable interrupts (if necessary) */
        ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

        if(MAX_REGIST_SRV_GEN_REQ <= i)
        {
            /* can't register */
            result = ER_K_ISSD_REGIST_FULL;
        }
        else
        {
            /* do nothing */
        }
    }
    return result;
}

/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0019]:[ICUMFW_UD_K_ISSD_0019]
* Function name: k_ISSD_GenerateServiceExec
* Description  : Generate service execute
* Arguments    : none
* Return Value : ER_OK
***************************************************************************************/
uint32_t k_ISSD_GenerateServiceExec(void)
{
    uint32_t i;
    uint32_t result;
    r_icumif_job_id_t new_job_id;
    uint32_t saved_psw;

    /* disable interrupts (if not already disabled) */
    DISABLE_INTERRUPT_WITH_CHECK(saved_psw);

    for(i = 0UL; i < MAX_REGIST_SRV_GEN_REQ; i++)
    {
        if(NULL != g_generate_ISD[i])
        {
            /* generate internal service */
            result = K_ISSD_AddService(g_generate_ISD[i], &new_job_id);
            if(ER_OK == result)
            {
                g_generate_ISD[i] = NULL;
                g_requested_gen_counter--;
                if(0UL == g_requested_gen_counter)
                {
                    break;
                }
                else
                {
                    /* Do nothing */
                }
            }
            else
            {
                /* if the job queue full */
                break;
            }
        }
        else
        {
            /* do nothing */
        }
    }
    /* re-enable interrupts (if necessary) */
    ENABLE_INTERRUPT_WITH_CHECK(saved_psw);

    return ER_OK;
}

CHANGE_SEC(text, ".icum_kernel_ram")
CHANGE_SEC(rodata, ".icum_kernel_const_ram")
/***************************************************************************************
* Function ID  : [ICUMFW_CD_K_ISSD_0020]:[ICUMFW_UD_K_ISSD_0020]
* Function name: K_ISSD_FastestCmacAddJobQue
* Description  : Adding a FastCMAC job in the job queue
* Arguments    : none
* Return Value : ER_OK
***************************************************************************************/
void K_ISSD_FastestCmacAddJobQue(icum_comm_int_from_channel_t channel, r_icumif_isd_t * p_isd)
{
    icum_job_t *p_ghost_job;
    p_service_func_t p_service_func;
    r_icumif_job_id_t new_job_id;
    r_icumif_service_result_t err;
    uint32_t result;
    
    err = SERV_REQUEST;
    new_job_id = 0u;
    
    /* NULL check and check channel is initial value */
    if ((NULL != p_isd) && (ICUM_COMM_ICH_FM_REGIST_ADDJOBFUNC == channel))
    {
        /* get service function address */
        result = k_ISSD_GetServiceFunc(p_isd->service_id, &p_service_func);
        
        if (ER_SERV_ID_ERROR == result)
        {
            err = SERV_ID_ERROR;
        }
        else
        {
            if (ER_SERV_UNAVAILABLE == result)
            {
                /* service not unavailable */
                err = SERV_UNAVAILABLE;
            }
            else if (ER_SERV_SYS_ERROR == result)
            {
                err = SERV_SYS_ERROR;
            }
            else
            {
                p_ghost_job = NULL;
                
                /* add job queue */
                k_ISSD_AddJobQueue(p_isd, p_ghost_job, &new_job_id);
                
                if ((uint32_t)0u == (uint32_t)new_job_id)
                {
                    /* job queue is full */
                    err = SERV_JOB_QUEUE_FULL;
                }
                else
                {
                    /* success add job queue */
                    p_isd->job_id = new_job_id;
                }
            }
        }
        
        /* set result to service result */
        p_isd->service_result = err;
    }
    else
    {
        /* Do nothing. (never occur) */
    }
}
CHANGE_SEC(text, default)
CHANGE_SEC(rodata, default)

