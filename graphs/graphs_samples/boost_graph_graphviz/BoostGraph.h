#pragma once

struct Edge
{
	// ���� �������� (��� �����).
	int64_t weight = 0;
};

struct Vertex
{
	// ��������� ����������� ���� �� ��������� ������� � ������� i.
	int64_t cost = 0;
	// ������ ���������� ������� �� ����������� ����.
	size_t previous = 0;
	// �������� ��������� ��������?
	bool isStart = 0;
};

class CBoostGraph
{
public:
	// "����������" ������� ���������, ������������ ��� ������ ��� ������������ ������.
	static const int64_t INFINITIVE_COST;

	CBoostGraph();
	~CBoostGraph();

	bool ReadGraphviz(std::istream & in);
	void RunBellmanFord(std::ostream & out);

private:
	using graph_t = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, Vertex, Edge>;
	using edge_iterator_t = boost::graph_traits<graph_t>::edge_iterator;
	using vertex_iterator_t = boost::graph_traits<graph_t>::vertex_iterator;

	class CNegativeLoopException : public std::exception
	{
	public:
		CNegativeLoopException(size_t vertexId);
		size_t m_vertexId;
	};

	void InitBellmanFordState();
	void PrintNegativeLoop(size_t vertexId, std::ostream & out);
	void PrintResults(std::ostream & out);
	bool UpdateBellmanFordState(bool isLastIteration);
	void GetShortestPath(size_t vertexId, std::vector<size_t> & shortestPath)const;

	graph_t m_graph;
	size_t m_startId = 0;
};

