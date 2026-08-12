tarefa Frente com numero velocidade = 100{
    TravarMotor("l", falso)
    TravarMotor("r", falso)
    TravarMotor("l1", falso)
    TravarMotor("r1", falso)

    Motor("l", velocidade)
    Motor("r", velocidade)
    Motor("l1", velocidade)
    Motor("r1", velocidade)
}

tarefa Direita com numero velocidade = 200{
    Motor("r", 0)
    Motor("r1", 0)
    TravarMotor("r", verdadeiro)
    TravarMotor("r1", verdadeiro)

    TravarMotor("l", falso)
    TravarMotor("l1", falso)
    Motor("l", velocidade * 2)
    Motor("l1", velocidade * 2)
}

tarefa Esquerda com numero velocidade = 200{
    Motor("l", 0)
    Motor("l1", 0)
    TravarMotor("l", verdadeiro)
    TravarMotor("l1", verdadeiro)

    TravarMotor("r", falso)
    TravarMotor("r1", falso)
    Motor("r", velocidade * 2)
    Motor("r1", velocidade * 2)
}

tarefa Tras com numero velocidade = 200{
    TravarMotor("l", falso)
    TravarMotor("r", falso)
    TravarMotor("l1", falso)
    TravarMotor("r1", falso)

    Motor("l", 0 - velocidade)
    Motor("r", 0 - velocidade)
    Motor("l1", 0 - velocidade)
    Motor("r1", 0 - velocidade)
}

tarefa Girar com numero velocidade = 100 {
    TravarMotor("l", falso)
    TravarMotor("r", falso)
    TravarMotor("l1", falso)
    TravarMotor("r1", falso)

    Motor("l", velocidade)
    Motor("r", 0 - velocidade)
    Motor("l1", velocidade)
    Motor("r1", 0 - velocidade)
}

tarefa Parar {
	Motor("l", 0)
    Motor("r", 0)
    Motor("l1", 0)
    Motor("r1", 0)

    TravarMotor("l", verdadeiro)
    TravarMotor("r", verdadeiro)
    TravarMotor("l1", verdadeiro)
    TravarMotor("r1", verdadeiro)
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
            Direita(400)
        } senao se((Cor("ld") != "Preto") e (Cor("le") == "Preto")) entao {
            LigarLuz("led", 255, 0, 0)
            Esquerda(400)
        } senao se((Cor("ld") == "Preto") e (Cor("le") == "Preto")) entao {
            LigarLuz("led", 0, 255, 0)
            Frente()
        } senao se((Cor("ld") == "Vermelho") ou (Cor("le") == "Vermelho")) entao {
            Frente(100)
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