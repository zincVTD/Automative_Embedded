typedef struct
{
  unsigned int CRL;
  unsigned int CRH;
  unsigned int IDR;
  unsigned int ODR;
  unsigned int BSRR;
  unsigned int BRR;
  unsigned int LCKR;
} GPIO_TypeDef;

typedef struct
{
  unsigned int CR;
  unsigned int CFGR;
  unsigned int CIR;
  unsigned int APB2RSTR;
  unsigned int APB1RSTR;
  unsigned int AHBENR;
  unsigned int APB2ENR;
  unsigned int APB1ENR;
  unsigned int BDCR;
  unsigned int CSR;
} RCC_TypeDef;

#define RCC		((RCC_TypeDef *)0x40021000)
#define GPIOA	((GPIO_TypeDef *)0x40010800)
#define GPIOC	((GPIO_TypeDef *)0x40011000)

void WritePin(GPIO_TypeDef *GPIO_Port, unsigned char Pin, unsigned char state)
{
	if(state == 1)
	GPIO_Port->ODR |= (1 << Pin);
	else
	GPIO_Port->ODR &= ~(1 << Pin);
}

void GPIO_Config(){
	//PC13
	GPIOC->CRH |= (1 << 20) | (1 << 21);			//MODE13 = 11
	GPIOC->CRH &= ~((1 << 22) | (1 << 23));		//CNF13 = 00
	
	//PA0
	GPIOA->CRL &= ~((1 << 0) | (1 << 1) | (1 << 2));
	GPIOA->CRL |= (1 << 3);
	GPIOA->ODR |= (1 << 0);
}

void delay(unsigned int timedelay){
	for(unsigned int i = 0; i < timedelay; i++);
}

int main(void){
	//Clock Enable
	RCC->APB2ENR |= (1 << 4) | (1 << 2);
	
	//Configure PC13
	GPIO_Config();
	
	while(1){
		if((GPIOA->IDR & (1 << 0)) == 0){
			WritePin(GPIOC, 13, 0);
		}
		else WritePin(GPIOC, 13, 1);
	}
}
