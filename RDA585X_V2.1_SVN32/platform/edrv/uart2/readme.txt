����˵����
1. UART2�꿪����target.def���棬UART2_SUPPORT������Ϊ1��֧��uart2������ر�uart2���ܡ�
2. bal_tasks.c��283�п�ʼ��������UARTָ�����Ӧ5850��Ϣ�Ľṹ�壬
Ԥ�ȼ��˼��������µĿ��԰������еĸ�ʽ������ӣ����⣬���ڲ�ͬ
ģ�����Ϣ�����ڲ�ͬ���ļ��У�Ϊ��ͳһ����event.h��284��������
��UART��Ϣ�Ķ��壬���е�UART��Ϣ��Ҫ��������ӣ�Ϊ����APP������
ģ�����ܴ�����Щ��Ϣ��������ԭ����������Ϣcase��������������UART
��Ϣ������
case UART_BT_VISIBLE://�����ӵ�UART��Ϣ
case BT_VISIBLE:

3. ���Ҫʹ��ģ����������Ҫ��ģ������event.h��Ҳ����UART��Ϣ���塣��Ȼ
ʹ��VC6.0����ᱨ��

4. ˯�߻��ѻ��ƣ�����Ҫ˯�ߵĵط�����hal_UartAllowSleep(HAL_UART_2, 1);�����ѵĵط�����hal_UartAllowSleep(HAL_UART_2, 0);
�����������̣�����ǰ���ѣ���������timer��˯�ߣ��������Ѿ�ʵ�֡�
�����������̣��ⲿ�豸��������ǰ����gpio����uart2��Ȼ�������ݣ�оƬ���������ݺ�����timer����˯�ߣ�
��Ҫ����TGT_GPIO_CONFIG_CFG�е�uart2_wakeΪ����gpio��

5. ��Ҫȷ��uart2��txd rxd�����ã���Ӧ��gpio(6,8)��Ҫ����ΪAS_ALT_FUNC��
// GPIO(6)  // UART2_TXD, UART1_CD
#define TGT_HAL_GPIO_6_USED AS_ALT_FUNC
// GPIO(8)  // UART2_RXD, UART1_DTR
#define TGT_HAL_GPIO_8_USED AS_ALT_FUNC