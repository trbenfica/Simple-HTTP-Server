# Simple HTTP Server

## Introdução

Servidor HTTP desenvolvido em C, como projeto da disciplina de Redes de Computadores da Universidade Federal do Pampa. O mesmo constitui um servidor simples mas funcional, possuindo algumas 
características interessantes, como conexões concorrentes, através da biblioteca Pthreads, keep-alive para clientes compatíveis, compatibilidade com arquivos CSS e jpg, entre outras.

## Instruções de uso

Para compilar o projeto, digite o seguinte comando dentro do diretório do projeto:
```console
$ make
```
Após, basta inserir a página HTML no diretório do projeto, e acessa-lá através de "http://localhost:9010/index.html". O servidor é compátivel apenas com arquivos HTML, CSS, JPG e JPEG.
Entretanto, qualquer expansão pode ser feita com facilidade, dada a simplicidade do código.

## Screenshots

Servidor esperando por requisições:

![image](https://user-images.githubusercontent.com/50810498/185488978-cfd01588-01f8-4336-abd9-e22c09ada6d3.png)

Após o envio de uma página ao cliente (neste caso, Firefox)
![image](https://user-images.githubusercontent.com/50810498/185488686-3e99870e-9aca-45e6-9419-7416d6bd3fcd.png)
