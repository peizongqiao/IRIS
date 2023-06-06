/**
 * \file
 *
 * \brief TCA related functionality implementation.
 *
 *
 * Copyright (C) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *
 */

#include <tca.h>

/**
 * \brief Initialize TCA interface
 */
int8_t TIMER_0_init()
{

	// TCA0.SINGLE.CMP0 = 0x0; /* Compare Register 0: 0x0 */

	// TCA0.SINGLE.CMP1 = 0x0; /* Compare Register 1: 0x0 */

	// TCA0.SINGLE.CMP2 = 0x0; /* Compare Register 2: 0x0 */

	 //TCA0.SINGLE.CNT = 0; /* Count: 0x0 */

	 //TCA0.SINGLE.CTRLB = 0 << TCA_SINGLE_ALUPD_bp /* Auto Lock Update: disabled */
			 //| 0 << TCA_SINGLE_CMP0EN_bp /* Compare 0 Enable: disabled */
			 //| 0 << TCA_SINGLE_CMP1EN_bp /* Compare 1 Enable: disabled */
			 //| 0 << TCA_SINGLE_CMP2EN_bp /* Compare 2 Enable: disabled */
			 //| TCA_SINGLE_WGMODE_SINGLESLOPE_gc; /*  */

	// TCA0.SINGLE.CTRLC = 0 << TCA_SINGLE_CMP0OV_bp /* Compare 0 Waveform Output Value: disabled */
	//		 | 0 << TCA_SINGLE_CMP1OV_bp /* Compare 1 Waveform Output Value: disabled */
	//		 | 0 << TCA_SINGLE_CMP2OV_bp; /* Compare 2 Waveform Output Value: disabled */

	// TCA0.SINGLE.DBGCTRL = 0 << TCA_SINGLE_DBGRUN_bp; /* Debug Run: disabled */

	// TCA0.SINGLE.EVCTRL = 0 << TCA_SINGLE_CNTEI_bp /* Count on Event Input: disabled */
	//		 | TCA_SINGLE_EVACT_POSEDGE_gc; /* Count on positive edge event */
	

	//TCA0.SINGLE.PER = 0xc3>>2; /* 80Hz */

	TCA0.SINGLE.CTRLA = TCA_SINGLE_CLKSEL_DIV1_gc
						| 1 << TCA_SINGLE_ENABLE_bp; /* Module Enable: enabled  Ö÷Æµ20M 2·ÖÆµ10M*/  
						
	TCA0.SINGLE.INTCTRL = 1 << TCA_SINGLE_OVF_bp; /* Overflow Interrupt: enabled */

	return 0;
}
