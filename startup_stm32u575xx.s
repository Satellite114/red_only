  .syntax unified
  .cpu cortex-m33
  .fpu fpv4-sp-d16
  .thumb

  .global  g_pfnVectors
  .global  Default_Handler

/* External functions ------------------------------------------------------- */
  .extern  SystemInit
  .extern  main

/* Cortex-M33 handlers ------------------------------------------------------ */
  .weak    NMI_Handler
  .weak    HardFault_Handler
  .weak    MemManage_Handler
  .weak    BusFault_Handler
  .weak    UsageFault_Handler
  .weak    SecureFault_Handler
  .weak    SVC_Handler
  .weak    DebugMon_Handler
  .weak    PendSV_Handler
  .weak    SysTick_Handler

/* STM32U575xx peripheral handlers (weak aliases to Default_Handler) -------- */
  .weak    WWDG_IRQHandler
  .weak    PVD_PVM_IRQHandler
  .weak    RTC_IRQHandler
  .weak    RTC_S_IRQHandler
  .weak    TAMP_IRQHandler
  .weak    RAMCFG_IRQHandler
  .weak    FLASH_IRQHandler
  .weak    FLASH_S_IRQHandler
  .weak    GTZC_IRQHandler
  .weak    RCC_IRQHandler
  .weak    RCC_S_IRQHandler
  .weak    EXTI0_IRQHandler
  .weak    EXTI1_IRQHandler
  .weak    EXTI2_IRQHandler
  .weak    EXTI3_IRQHandler
  .weak    EXTI4_IRQHandler
  .weak    EXTI5_IRQHandler
  .weak    EXTI6_IRQHandler
  .weak    EXTI7_IRQHandler
  .weak    EXTI8_IRQHandler
  .weak    EXTI9_IRQHandler
  .weak    EXTI10_IRQHandler
  .weak    EXTI11_IRQHandler
  .weak    EXTI12_IRQHandler
  .weak    EXTI13_IRQHandler
  .weak    EXTI14_IRQHandler
  .weak    EXTI15_IRQHandler
  .weak    IWDG_IRQHandler
  .weak    GPDMA1_Channel0_IRQHandler
  .weak    GPDMA1_Channel1_IRQHandler
  .weak    GPDMA1_Channel2_IRQHandler
  .weak    GPDMA1_Channel3_IRQHandler
  .weak    GPDMA1_Channel4_IRQHandler
  .weak    GPDMA1_Channel5_IRQHandler
  .weak    GPDMA1_Channel6_IRQHandler
  .weak    GPDMA1_Channel7_IRQHandler
  .weak    ADC1_IRQHandler
  .weak    DAC1_IRQHandler
  .weak    FDCAN1_IT0_IRQHandler
  .weak    FDCAN1_IT1_IRQHandler
  .weak    TIM1_BRK_IRQHandler
  .weak    TIM1_UP_IRQHandler
  .weak    TIM1_TRG_COM_IRQHandler
  .weak    TIM1_CC_IRQHandler
  .weak    TIM2_IRQHandler
  .weak    TIM3_IRQHandler
  .weak    TIM4_IRQHandler
  .weak    TIM5_IRQHandler
  .weak    TIM6_IRQHandler
  .weak    TIM7_IRQHandler
  .weak    TIM8_BRK_IRQHandler
  .weak    TIM8_UP_IRQHandler
  .weak    TIM8_TRG_COM_IRQHandler
  .weak    TIM8_CC_IRQHandler
  .weak    I2C1_EV_IRQHandler
  .weak    I2C1_ER_IRQHandler
  .weak    I2C2_EV_IRQHandler
  .weak    I2C2_ER_IRQHandler
  .weak    SPI1_IRQHandler
  .weak    SPI2_IRQHandler
  .weak    USART1_IRQHandler
  .weak    USART2_IRQHandler
  .weak    USART3_IRQHandler
  .weak    UART4_IRQHandler
  .weak    UART5_IRQHandler
  .weak    LPUART1_IRQHandler
  .weak    LPTIM1_IRQHandler
  .weak    LPTIM2_IRQHandler
  .weak    TIM15_IRQHandler
  .weak    TIM16_IRQHandler
  .weak    TIM17_IRQHandler
  .weak    COMP_IRQHandler
  .weak    OTG_FS_IRQHandler
  .weak    CRS_IRQHandler
  .weak    FMC_IRQHandler
  .weak    OCTOSPI1_IRQHandler
  .weak    PWR_S3WU_IRQHandler
  .weak    SDMMC1_IRQHandler
  .weak    SDMMC2_IRQHandler
  .weak    GPDMA1_Channel8_IRQHandler
  .weak    GPDMA1_Channel9_IRQHandler
  .weak    GPDMA1_Channel10_IRQHandler
  .weak    GPDMA1_Channel11_IRQHandler
  .weak    GPDMA1_Channel12_IRQHandler
  .weak    GPDMA1_Channel13_IRQHandler
  .weak    GPDMA1_Channel14_IRQHandler
  .weak    GPDMA1_Channel15_IRQHandler
  .weak    I2C3_EV_IRQHandler
  .weak    I2C3_ER_IRQHandler
  .weak    SAI1_IRQHandler
  .weak    SAI2_IRQHandler
  .weak    TSC_IRQHandler
  .weak    RNG_IRQHandler
  .weak    FPU_IRQHandler
  .weak    HASH_IRQHandler
  .weak    LPTIM3_IRQHandler
  .weak    SPI3_IRQHandler
  .weak    I2C4_ER_IRQHandler
  .weak    I2C4_EV_IRQHandler
  .weak    MDF1_FLT0_IRQHandler
  .weak    MDF1_FLT1_IRQHandler
  .weak    MDF1_FLT2_IRQHandler
  .weak    MDF1_FLT3_IRQHandler
  .weak    UCPD1_IRQHandler
  .weak    ICACHE_IRQHandler
  .weak    LPTIM4_IRQHandler
  .weak    DCACHE1_IRQHandler
  .weak    ADF1_IRQHandler
  .weak    ADC4_IRQHandler
  .weak    LPDMA1_Channel0_IRQHandler
  .weak    LPDMA1_Channel1_IRQHandler
  .weak    LPDMA1_Channel2_IRQHandler
  .weak    LPDMA1_Channel3_IRQHandler
  .weak    DMA2D_IRQHandler
  .weak    DCMI_PSSI_IRQHandler
  .weak    OCTOSPI2_IRQHandler
  .weak    MDF1_FLT4_IRQHandler
  .weak    MDF1_FLT5_IRQHandler
  .weak    CORDIC_IRQHandler
  .weak    FMAC_IRQHandler
  .weak    LSECSSD_IRQHandler

  .thumb_set NMI_Handler, Default_Handler
  .thumb_set HardFault_Handler, Default_Handler
  .thumb_set MemManage_Handler, Default_Handler
  .thumb_set BusFault_Handler, Default_Handler
  .thumb_set UsageFault_Handler, Default_Handler
  .thumb_set SecureFault_Handler, Default_Handler
  .thumb_set SVC_Handler, Default_Handler
  .thumb_set DebugMon_Handler, Default_Handler
  .thumb_set PendSV_Handler, Default_Handler
  .thumb_set SysTick_Handler, Default_Handler

  .thumb_set WWDG_IRQHandler, Default_Handler
  .thumb_set PVD_PVM_IRQHandler, Default_Handler
  .thumb_set RTC_IRQHandler, Default_Handler
  .thumb_set RTC_S_IRQHandler, Default_Handler
  .thumb_set TAMP_IRQHandler, Default_Handler
  .thumb_set RAMCFG_IRQHandler, Default_Handler
  .thumb_set FLASH_IRQHandler, Default_Handler
  .thumb_set FLASH_S_IRQHandler, Default_Handler
  .thumb_set GTZC_IRQHandler, Default_Handler
  .thumb_set RCC_IRQHandler, Default_Handler
  .thumb_set RCC_S_IRQHandler, Default_Handler
  .thumb_set EXTI0_IRQHandler, Default_Handler
  .thumb_set EXTI1_IRQHandler, Default_Handler
  .thumb_set EXTI2_IRQHandler, Default_Handler
  .thumb_set EXTI3_IRQHandler, Default_Handler
  .thumb_set EXTI4_IRQHandler, Default_Handler
  .thumb_set EXTI5_IRQHandler, Default_Handler
  .thumb_set EXTI6_IRQHandler, Default_Handler
  .thumb_set EXTI7_IRQHandler, Default_Handler
  .thumb_set EXTI8_IRQHandler, Default_Handler
  .thumb_set EXTI9_IRQHandler, Default_Handler
  .thumb_set EXTI10_IRQHandler, Default_Handler
  .thumb_set EXTI11_IRQHandler, Default_Handler
  .thumb_set EXTI12_IRQHandler, Default_Handler
  .thumb_set EXTI13_IRQHandler, Default_Handler
  .thumb_set EXTI14_IRQHandler, Default_Handler
  .thumb_set EXTI15_IRQHandler, Default_Handler
  .thumb_set IWDG_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel0_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel1_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel2_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel3_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel4_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel5_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel6_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel7_IRQHandler, Default_Handler
  .thumb_set ADC1_IRQHandler, Default_Handler
  .thumb_set DAC1_IRQHandler, Default_Handler
  .thumb_set FDCAN1_IT0_IRQHandler, Default_Handler
  .thumb_set FDCAN1_IT1_IRQHandler, Default_Handler
  .thumb_set TIM1_BRK_IRQHandler, Default_Handler
  .thumb_set TIM1_UP_IRQHandler, Default_Handler
  .thumb_set TIM1_TRG_COM_IRQHandler, Default_Handler
  .thumb_set TIM1_CC_IRQHandler, Default_Handler
  .thumb_set TIM2_IRQHandler, Default_Handler
  .thumb_set TIM3_IRQHandler, Default_Handler
  .thumb_set TIM4_IRQHandler, Default_Handler
  .thumb_set TIM5_IRQHandler, Default_Handler
  .thumb_set TIM6_IRQHandler, Default_Handler
  .thumb_set TIM7_IRQHandler, Default_Handler
  .thumb_set TIM8_BRK_IRQHandler, Default_Handler
  .thumb_set TIM8_UP_IRQHandler, Default_Handler
  .thumb_set TIM8_TRG_COM_IRQHandler, Default_Handler
  .thumb_set TIM8_CC_IRQHandler, Default_Handler
  .thumb_set I2C1_EV_IRQHandler, Default_Handler
  .thumb_set I2C1_ER_IRQHandler, Default_Handler
  .thumb_set I2C2_EV_IRQHandler, Default_Handler
  .thumb_set I2C2_ER_IRQHandler, Default_Handler
  .thumb_set SPI1_IRQHandler, Default_Handler
  .thumb_set SPI2_IRQHandler, Default_Handler
  .thumb_set USART1_IRQHandler, Default_Handler
  .thumb_set USART2_IRQHandler, Default_Handler
  .thumb_set USART3_IRQHandler, Default_Handler
  .thumb_set UART4_IRQHandler, Default_Handler
  .thumb_set UART5_IRQHandler, Default_Handler
  .thumb_set LPUART1_IRQHandler, Default_Handler
  .thumb_set LPTIM1_IRQHandler, Default_Handler
  .thumb_set LPTIM2_IRQHandler, Default_Handler
  .thumb_set TIM15_IRQHandler, Default_Handler
  .thumb_set TIM16_IRQHandler, Default_Handler
  .thumb_set TIM17_IRQHandler, Default_Handler
  .thumb_set COMP_IRQHandler, Default_Handler
  .thumb_set OTG_FS_IRQHandler, Default_Handler
  .thumb_set CRS_IRQHandler, Default_Handler
  .thumb_set FMC_IRQHandler, Default_Handler
  .thumb_set OCTOSPI1_IRQHandler, Default_Handler
  .thumb_set PWR_S3WU_IRQHandler, Default_Handler
  .thumb_set SDMMC1_IRQHandler, Default_Handler
  .thumb_set SDMMC2_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel8_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel9_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel10_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel11_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel12_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel13_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel14_IRQHandler, Default_Handler
  .thumb_set GPDMA1_Channel15_IRQHandler, Default_Handler
  .thumb_set I2C3_EV_IRQHandler, Default_Handler
  .thumb_set I2C3_ER_IRQHandler, Default_Handler
  .thumb_set SAI1_IRQHandler, Default_Handler
  .thumb_set SAI2_IRQHandler, Default_Handler
  .thumb_set TSC_IRQHandler, Default_Handler
  .thumb_set RNG_IRQHandler, Default_Handler
  .thumb_set FPU_IRQHandler, Default_Handler
  .thumb_set HASH_IRQHandler, Default_Handler
  .thumb_set LPTIM3_IRQHandler, Default_Handler
  .thumb_set SPI3_IRQHandler, Default_Handler
  .thumb_set I2C4_ER_IRQHandler, Default_Handler
  .thumb_set I2C4_EV_IRQHandler, Default_Handler
  .thumb_set MDF1_FLT0_IRQHandler, Default_Handler
  .thumb_set MDF1_FLT1_IRQHandler, Default_Handler
  .thumb_set MDF1_FLT2_IRQHandler, Default_Handler
  .thumb_set MDF1_FLT3_IRQHandler, Default_Handler
  .thumb_set UCPD1_IRQHandler, Default_Handler
  .thumb_set ICACHE_IRQHandler, Default_Handler
  .thumb_set LPTIM4_IRQHandler, Default_Handler
  .thumb_set DCACHE1_IRQHandler, Default_Handler
  .thumb_set ADF1_IRQHandler, Default_Handler
  .thumb_set ADC4_IRQHandler, Default_Handler
  .thumb_set LPDMA1_Channel0_IRQHandler, Default_Handler
  .thumb_set LPDMA1_Channel1_IRQHandler, Default_Handler
  .thumb_set LPDMA1_Channel2_IRQHandler, Default_Handler
  .thumb_set LPDMA1_Channel3_IRQHandler, Default_Handler
  .thumb_set DMA2D_IRQHandler, Default_Handler
  .thumb_set DCMI_PSSI_IRQHandler, Default_Handler
  .thumb_set OCTOSPI2_IRQHandler, Default_Handler
  .thumb_set MDF1_FLT4_IRQHandler, Default_Handler
  .thumb_set MDF1_FLT5_IRQHandler, Default_Handler
  .thumb_set CORDIC_IRQHandler, Default_Handler
  .thumb_set FMAC_IRQHandler, Default_Handler
  .thumb_set LSECSSD_IRQHandler, Default_Handler

/* Vector Table ------------------------------------------------------------- */
  .section  .isr_vector,"a",%progbits
g_pfnVectors:
  .word  _estack
  .word  Reset_Handler
  .word  NMI_Handler
  .word  HardFault_Handler
  .word  MemManage_Handler
  .word  BusFault_Handler
  .word  UsageFault_Handler
  .word  SecureFault_Handler
  .word  0
  .word  0
  .word  0
  .word  SVC_Handler
  .word  DebugMon_Handler
  .word  0
  .word  PendSV_Handler
  .word  SysTick_Handler

  /* External Interrupts (IRQn >= 0) */
  .word  WWDG_IRQHandler                   /* 0 */
  .word  PVD_PVM_IRQHandler                /* 1 */
  .word  RTC_IRQHandler                    /* 2 */
  .word  RTC_S_IRQHandler                  /* 3 */
  .word  TAMP_IRQHandler                   /* 4 */
  .word  RAMCFG_IRQHandler                 /* 5 */
  .word  FLASH_IRQHandler                  /* 6 */
  .word  FLASH_S_IRQHandler                /* 7 */
  .word  GTZC_IRQHandler                   /* 8 */
  .word  RCC_IRQHandler                    /* 9 */
  .word  RCC_S_IRQHandler                  /* 10 */
  .word  EXTI0_IRQHandler                  /* 11 */
  .word  EXTI1_IRQHandler                  /* 12 */
  .word  EXTI2_IRQHandler                  /* 13 */
  .word  EXTI3_IRQHandler                  /* 14 */
  .word  EXTI4_IRQHandler                  /* 15 */
  .word  EXTI5_IRQHandler                  /* 16 */
  .word  EXTI6_IRQHandler                  /* 17 */
  .word  EXTI7_IRQHandler                  /* 18 */
  .word  EXTI8_IRQHandler                  /* 19 */
  .word  EXTI9_IRQHandler                  /* 20 */
  .word  EXTI10_IRQHandler                 /* 21 */
  .word  EXTI11_IRQHandler                 /* 22 */
  .word  EXTI12_IRQHandler                 /* 23 */
  .word  EXTI13_IRQHandler                 /* 24 */
  .word  EXTI14_IRQHandler                 /* 25 */
  .word  EXTI15_IRQHandler                 /* 26 */
  .word  IWDG_IRQHandler                   /* 27 */
  .word  0                                 /* 28 Reserved */
  .word  GPDMA1_Channel0_IRQHandler        /* 29 */
  .word  GPDMA1_Channel1_IRQHandler        /* 30 */
  .word  GPDMA1_Channel2_IRQHandler        /* 31 */
  .word  GPDMA1_Channel3_IRQHandler        /* 32 */
  .word  GPDMA1_Channel4_IRQHandler        /* 33 */
  .word  GPDMA1_Channel5_IRQHandler        /* 34 */
  .word  GPDMA1_Channel6_IRQHandler        /* 35 */
  .word  GPDMA1_Channel7_IRQHandler        /* 36 */
  .word  ADC1_IRQHandler                   /* 37 */
  .word  DAC1_IRQHandler                   /* 38 */
  .word  FDCAN1_IT0_IRQHandler             /* 39 */
  .word  FDCAN1_IT1_IRQHandler             /* 40 */
  .word  TIM1_BRK_IRQHandler               /* 41 */
  .word  TIM1_UP_IRQHandler                /* 42 */
  .word  TIM1_TRG_COM_IRQHandler           /* 43 */
  .word  TIM1_CC_IRQHandler                /* 44 */
  .word  TIM2_IRQHandler                   /* 45 */
  .word  TIM3_IRQHandler                   /* 46 */
  .word  TIM4_IRQHandler                   /* 47 */
  .word  TIM5_IRQHandler                   /* 48 */
  .word  TIM6_IRQHandler                   /* 49 */
  .word  TIM7_IRQHandler                   /* 50 */
  .word  TIM8_BRK_IRQHandler               /* 51 */
  .word  TIM8_UP_IRQHandler                /* 52 */
  .word  TIM8_TRG_COM_IRQHandler           /* 53 */
  .word  TIM8_CC_IRQHandler                /* 54 */
  .word  I2C1_EV_IRQHandler                /* 55 */
  .word  I2C1_ER_IRQHandler                /* 56 */
  .word  I2C2_EV_IRQHandler                /* 57 */
  .word  I2C2_ER_IRQHandler                /* 58 */
  .word  SPI1_IRQHandler                   /* 59 */
  .word  SPI2_IRQHandler                   /* 60 */
  .word  USART1_IRQHandler                 /* 61 */
  .word  USART2_IRQHandler                 /* 62 */
  .word  USART3_IRQHandler                 /* 63 */
  .word  UART4_IRQHandler                  /* 64 */
  .word  UART5_IRQHandler                  /* 65 */
  .word  LPUART1_IRQHandler                /* 66 */
  .word  LPTIM1_IRQHandler                 /* 67 */
  .word  LPTIM2_IRQHandler                 /* 68 */
  .word  TIM15_IRQHandler                  /* 69 */
  .word  TIM16_IRQHandler                  /* 70 */
  .word  TIM17_IRQHandler                  /* 71 */
  .word  COMP_IRQHandler                   /* 72 */
  .word  OTG_FS_IRQHandler                 /* 73 */
  .word  CRS_IRQHandler                    /* 74 */
  .word  FMC_IRQHandler                    /* 75 */
  .word  OCTOSPI1_IRQHandler               /* 76 */
  .word  PWR_S3WU_IRQHandler               /* 77 */
  .word  SDMMC1_IRQHandler                 /* 78 */
  .word  SDMMC2_IRQHandler                 /* 79 */
  .word  GPDMA1_Channel8_IRQHandler        /* 80 */
  .word  GPDMA1_Channel9_IRQHandler        /* 81 */
  .word  GPDMA1_Channel10_IRQHandler       /* 82 */
  .word  GPDMA1_Channel11_IRQHandler       /* 83 */
  .word  GPDMA1_Channel12_IRQHandler       /* 84 */
  .word  GPDMA1_Channel13_IRQHandler       /* 85 */
  .word  GPDMA1_Channel14_IRQHandler       /* 86 */
  .word  GPDMA1_Channel15_IRQHandler       /* 87 */
  .word  I2C3_EV_IRQHandler                /* 88 */
  .word  I2C3_ER_IRQHandler                /* 89 */
  .word  SAI1_IRQHandler                   /* 90 */
  .word  SAI2_IRQHandler                   /* 91 */
  .word  TSC_IRQHandler                    /* 92 */
  .word  0                                 /* 93 Reserved */
  .word  RNG_IRQHandler                    /* 94 */
  .word  FPU_IRQHandler                    /* 95 */
  .word  HASH_IRQHandler                   /* 96 */
  .word  0                                 /* 97 Reserved */
  .word  LPTIM3_IRQHandler                 /* 98 */
  .word  SPI3_IRQHandler                   /* 99 */
  .word  I2C4_ER_IRQHandler                /* 100 */
  .word  I2C4_EV_IRQHandler                /* 101 */
  .word  MDF1_FLT0_IRQHandler              /* 102 */
  .word  MDF1_FLT1_IRQHandler              /* 103 */
  .word  MDF1_FLT2_IRQHandler              /* 104 */
  .word  MDF1_FLT3_IRQHandler              /* 105 */
  .word  UCPD1_IRQHandler                  /* 106 */
  .word  ICACHE_IRQHandler                 /* 107 */
  .word  0                                 /* 108 Reserved */
  .word  0                                 /* 109 Reserved */
  .word  LPTIM4_IRQHandler                 /* 110 */
  .word  DCACHE1_IRQHandler                /* 111 */
  .word  ADF1_IRQHandler                   /* 112 */
  .word  ADC4_IRQHandler                   /* 113 */
  .word  LPDMA1_Channel0_IRQHandler        /* 114 */
  .word  LPDMA1_Channel1_IRQHandler        /* 115 */
  .word  LPDMA1_Channel2_IRQHandler        /* 116 */
  .word  LPDMA1_Channel3_IRQHandler        /* 117 */
  .word  DMA2D_IRQHandler                  /* 118 */
  .word  DCMI_PSSI_IRQHandler              /* 119 */
  .word  OCTOSPI2_IRQHandler               /* 120 */
  .word  MDF1_FLT4_IRQHandler              /* 121 */
  .word  MDF1_FLT5_IRQHandler              /* 122 */
  .word  CORDIC_IRQHandler                 /* 123 */
  .word  FMAC_IRQHandler                   /* 124 */
  .word  LSECSSD_IRQHandler                /* 125 */

/* Reset handler ----------------------------------------------------------- */
  .text
  .thumb
  .thumb_func
Reset_Handler:
  ldr   r0, =_estack
  mov   sp, r0
  bl    SystemInit
  bl    main
Infinite_Loop:
  b Infinite_Loop

/* Default handler --------------------------------------------------------- */
  .thumb_func
Default_Handler:
  b Infinite_Loop
