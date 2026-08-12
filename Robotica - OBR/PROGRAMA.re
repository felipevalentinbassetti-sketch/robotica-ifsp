tarefa Frente com numero velocidade = 100{
        TravarMotor("l", falso) # Destrava o motor da esquerda
        TravarMotor("r", falso) # Destrava o motor da direita

        Motor("l", velocidade)
        Motor("r", velocidade)
}

tarefa Direita com numero velocidade = 250{
        Motor("r", 0)
        TravarMotor("r", verdadeiro) # Trava o motor da direita

        TravarMotor("l", falso) # Destrava o motor da esquerda
        Motor("l", velocidade*2)
}

tarefa Esquerda com numero velocidade = 250{
        Motor("l", 0)
        TravarMotor("l", verdadeiro) # Trava o motor da esquerda

        TravarMotor("r", falso) # Destrava o motor da direita
        Motor("r", velocidade*2)
}

tarefa Tras com numero velocidade = 200{
        TravarMotor("l", falso) # Destrava o motor da esquerda
        TravarMotor("r", falso) # Destrava o motor da direita

        Motor("l", 0-velocidade)
        Motor("r", 0-velocidade)
}

tarefa Girar com numero velocidade = 100 {
    TravarMotor("l", falso)
    TravarMotor("r", falso)
    
    Motor("l", velocidade)
    Motor("r", 0 - velocidade)
}

tarefa Parar {
    Motor("l", 0)
    Motor("r", 0)
    TravarMotor("l", verdadeiro)
    TravarMotor("r", verdadeiro)
}

tarefa BuscarBola com numero velocidade = 200{
    enquanto (verdadeiro) farei {
        se (Cor("fd") == "branco" e (Cor("fe") != "Preto")) entao { 
            Parar()
            Frente()
        } senao {
            Girar(30) 
        }
    }
}
numero contador = 0
tarefa LedFinal {
    para contador de 1 ate 5 passo 1 farei {
        LigarLuz("led", 255, 0, 0)
        Esperar(50)
        LigarLuz("led", 0, 255, 0)
        Esperar(50)
        LigarLuz("led", 0, 0, 255)
        Esperar(50)
    }
}

inicio
    AbrirConsole()
    
    enquanto (verdadeiro) farei {
        EscreverLinha("Escrevendo no Console")
        se((Cor("ld") == "Preto") e (Cor("le") != "Preto")) entao {
            LigarLuz("led", 0, 0, 255)
            Direita(300)
        } senao se((Cor("ld") != "Preto") e (Cor("le") == "Preto")) entao {
            LigarLuz("led", 255, 0, 0)
            Esquerda(300)
        } senao se((Cor("ld") == "Preto") e (Cor("le") == "Preto")) entao {
            LigarLuz("led", 0, 255, 0)
            Frente()
        } senao se((Cor("ld") == "Vermelho") ou (Cor("le") == "Vermelho")) entao {
            Frente(0)
            LedFinal()
        } senao se((Cor("ld") == "Branco") e (Cor("le") == "branco")) entao {
            BuscarBola()
            LedFinal()
        } senao { 
            LigarLuz("led", 255, 0, 255)
            Frente()
        }
    }

fim