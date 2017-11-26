/*******************************************************************************
* File Name: ChannelSel.c
* Version 1.80
*
*  Description:
*    This file contains all functions required for the analog multiplexer
*    AMux User Module.
*
*   Note:
*
*******************************************************************************
* Copyright 2008-2010, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
********************************************************************************/

#include "ChannelSel.h"

static uint8 ChannelSel_lastChannel = ChannelSel_NULL_CHANNEL;


/*******************************************************************************
* Function Name: ChannelSel_Start
********************************************************************************
* Summary:
*  Disconnect all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void ChannelSel_Start(void) 
{
    uint8 chan;

    for(chan = 0u; chan < ChannelSel_CHANNELS ; chan++)
    {
#if (ChannelSel_MUXTYPE == ChannelSel_MUX_SINGLE)
        ChannelSel_Unset(chan);
#else
        ChannelSel_CYAMUXSIDE_A_Unset(chan);
        ChannelSel_CYAMUXSIDE_B_Unset(chan);
#endif
    }

    ChannelSel_lastChannel = ChannelSel_NULL_CHANNEL;
}


#if (!ChannelSel_ATMOSTONE)
/*******************************************************************************
* Function Name: ChannelSel_Select
********************************************************************************
* Summary:
*  This functions first disconnects all channels then connects the given
*  channel.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void ChannelSel_Select(uint8 channel) 
{
    ChannelSel_DisconnectAll();        /* Disconnect all previous connections */
    ChannelSel_Connect(channel);       /* Make the given selection */
    ChannelSel_lastChannel = channel;  /* Update last channel */
}
#endif


/*******************************************************************************
* Function Name: ChannelSel_FastSelect
********************************************************************************
* Summary:
*  This function first disconnects the last connection made with FastSelect or
*  Select, then connects the given channel. The FastSelect function is similar
*  to the Select function, except it is faster since it only disconnects the
*  last channel selected rather than all channels.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void ChannelSel_FastSelect(uint8 channel) 
{
    /* Disconnect the last valid channel */
    if( ChannelSel_lastChannel != ChannelSel_NULL_CHANNEL)
    {
        ChannelSel_Disconnect(ChannelSel_lastChannel);
    }

    /* Make the new channel connection */
#if (ChannelSel_MUXTYPE == ChannelSel_MUX_SINGLE)
    ChannelSel_Set(channel);
#else
    ChannelSel_CYAMUXSIDE_A_Set(channel);
    ChannelSel_CYAMUXSIDE_B_Set(channel);
#endif


    ChannelSel_lastChannel = channel;   /* Update last channel */
}


#if (ChannelSel_MUXTYPE == ChannelSel_MUX_DIFF)
#if (!ChannelSel_ATMOSTONE)
/*******************************************************************************
* Function Name: ChannelSel_Connect
********************************************************************************
* Summary:
*  This function connects the given channel without affecting other connections.
*
* Parameters:
*  channel:  The channel to connect to the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void ChannelSel_Connect(uint8 channel) 
{
    ChannelSel_CYAMUXSIDE_A_Set(channel);
    ChannelSel_CYAMUXSIDE_B_Set(channel);
}
#endif

/*******************************************************************************
* Function Name: ChannelSel_Disconnect
********************************************************************************
* Summary:
*  This function disconnects the given channel from the common or output
*  terminal without affecting other connections.
*
* Parameters:
*  channel:  The channel to disconnect from the common terminal.
*
* Return:
*  void
*
*******************************************************************************/
void ChannelSel_Disconnect(uint8 channel) 
{
    ChannelSel_CYAMUXSIDE_A_Unset(channel);
    ChannelSel_CYAMUXSIDE_B_Unset(channel);
}
#endif

#if (ChannelSel_ATMOSTONE)
/*******************************************************************************
* Function Name: ChannelSel_DisconnectAll
********************************************************************************
* Summary:
*  This function disconnects all channels.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void ChannelSel_DisconnectAll(void) 
{
    if(ChannelSel_lastChannel != ChannelSel_NULL_CHANNEL) 
    {
        ChannelSel_Disconnect(ChannelSel_lastChannel);
        ChannelSel_lastChannel = ChannelSel_NULL_CHANNEL;
    }
}
#endif

/* [] END OF FILE */
