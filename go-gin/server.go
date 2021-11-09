package main

import (
	"fmt"

	"github.com/gin-gonic/gin"
)

type scfHeader struct {
	ScfRequestId     string `header:"X-Scf-Request-Id"`
}

type json struct {
	Key string `json:"key"`
}

func scfWebInvoke(context *gin.Context) {

	ScfRequestId := context.GetHeader("X-Scf-Request-Id")
	context.JSON(200, gin.H{
		"message": fmt.Sprintf("Hello Gin, from scf web function!\nPowerd by Serverless Cloud Function\nRequestID: %s\n", ScfRequestId),
	})
}

func scfEventInvoke(context *gin.Context) {

	ScfRequestId := context.GetHeader("X-Scf-Request-Id")
	context.JSON(200, gin.H{
		"message": fmt.Sprintf("Hello Gin, from scf event function!\nPowerd by Serverless Cloud Function\nRequestID: %s\n", ScfRequestId),
	})
}

func main() {
	router := gin.Default()

	// SCF API Gateway trigger
	router.GET("/", scfWebInvoke)

	// SCF Event trigeer
	router.POST("/event-invoke", scfEventInvoke)

	// listen and serve on port 9000
	router.Run(":9000")
}
