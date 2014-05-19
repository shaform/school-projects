package main

import (
	"bufio"
	"flag"
	"fmt"
	"math"
	"os"
	"time"
)

const (
	DP    = 0.85
	ESP   = 1e-6
	OFILE = "output.pagerank"
)

type Graph struct {
	inEdges    [][]int
	outDegree  []int
	emptyNodes []int
}

func NewGraph(size int) *Graph {
	return &Graph{make([][]int, size), make([]int, size), []int{}}
}

func constructGraph(fnGraph string) *Graph {
	fGraph, err := os.Open(fnGraph)
	if err != nil {
		fmt.Println("cannot open file to read! filename:", fnGraph)
		fmt.Println(err)
		os.Exit(1)
	}

	bGraph := bufio.NewReader(fGraph)
	size := 0
	for {
		var prefix string
		_, err := fmt.Fscan(bGraph, &prefix)
		if err != nil {
			panic(err)
		}
		if prefix[0] == '#' {
			_, err := fmt.Fscan(bGraph, &size)
			if err != nil {
				panic(err)
			}
			break
		}
	}

	g := NewGraph(size)

	// read all edges
	for {
		var idx, outDeg int
		_, err := fmt.Fscan(bGraph, &idx)
		if err != nil {
			break
		} else {
			_, err := fmt.Fscanf(bGraph, ":%d", &outDeg)
			if err != nil {
				fmt.Println(err)
				break
			}
		}
		idx--
		g.outDegree[idx] = outDeg
		for i := 0; i < outDeg; i++ {
			var oIdx int
			fmt.Fscan(bGraph, &oIdx)
			g.inEdges[oIdx-1] = append(g.inEdges[oIdx-1], idx)
		}
	}

	if err := fGraph.Close(); err != nil {
		fmt.Println("cannot close file!")
		fmt.Println(err)
		os.Exit(1)
	}

	// record all nodes with 0 out degree
	for idx, dg := range g.outDegree {
		if dg == 0 {
			g.emptyNodes = append(g.emptyNodes, idx)
			g.outDegree[idx] = size
		}
	}
	return g
}

type Prob struct {
	idx       int
	outDegree float64
	inWeights []float64
}

type Ans struct {
	idx    int
	pg     float64
	weight float64
}

// 3.65999358s
func pagerank(g *Graph, d float64, eps float64) []float64 {
	glen := len(g.outDegree)
	pg1, pg2 := make([]float64, glen), make([]float64, glen)
	weight1, weight2 := make([]float64, glen), make([]float64, glen)
	// initialize pagerank
	for i := range pg1 {
		pg1[i] = float64(1.0)
		weight1[i] = float64(1.0) / float64(g.outDegree[i])
	}

	// power iteration
	for {
		totalE := float64(0.0)
		for _, idx := range g.emptyNodes {
			totalE += weight1[idx]
		}

		diff := float64(0.0)
		for i := range pg2 {
			w := totalE
			for _, idx := range g.inEdges[i] {
				w += weight1[idx]
			}
			pg2[i] = (1.0 - d) + d*w
			weight2[i] = pg2[i] / float64(g.outDegree[i])
			diff += math.Pow(pg1[i]-pg2[i], 2)
		}

		if math.Sqrt(diff) < eps {
			break
		}
		pg1, pg2 = pg2, pg1
		weight1, weight2 = weight2, weight1
	}

	return pg2
}

func outputPagerank(pg []float64, fnRank string) {
	fRank, err := os.Create(fnRank)
	if err != nil {
		fmt.Println("cannot create file to write! filename:", fnRank)
		fmt.Println(err)
		os.Exit(1)
	}
	for i, v := range pg {
		fmt.Fprintf(fRank, "%d:%f\n", i+1, v)
	}
	if err := fRank.Close(); err != nil {
		fmt.Println("cannot close file!")
		fmt.Println(err)
		os.Exit(1)
	}
}

func main() {
	// process command line arguments
	dPtr := flag.Float64("d", DP, "damping factor")
	epsPtr := flag.Float64("e", ESP, "epsilon")
	outputPtr := flag.String("o", OFILE, "output file")
	flag.Parse()

	if flag.NArg() == 0 {
		fmt.Println("please specify input file")
		os.Exit(1)
	}
	fnGraph := flag.Arg(0)
	fnRank := *outputPtr
	dNum := *dPtr
	eps := *epsPtr

	// read the file

	fmt.Println("constructing graph...")
	g := constructGraph(fnGraph)

	//

	fmt.Println("start pageranking...")

	startTime := time.Now()
	pg := pagerank(g, dNum, eps)
	endTime := time.Now()

	fmt.Println("Time spent:", endTime.Sub(startTime))

	//

	fmt.Println("store result...")
	outputPagerank(pg, fnRank)
}
