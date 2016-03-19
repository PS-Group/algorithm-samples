#pragma once

#include <stdint.h>
#include <iostream>
#include <boost/variant.hpp>

struct Edge
{
	// ������ 1-� ������� (������ �����).
	size_t start = 0;
	// ������ 2-� ������� (����� �����).
	size_t end = 0;
	// ���� �������� (��� �����).
	int64_t weight = 0;
};

struct Vertex
{
	// ��������� ����������� ���� �� ��������� ������� � ������� i.
	int64_t cost = 0;
	// ������ ���������� ������� �� ���������� ����.
	size_t previous = 0;
};

class CGraph
{
public:
	static const int64_t INFINITIVE_COST;

	CGraph();
	~CGraph();

	void ReadText(std::istream & in);
	void RunBellmanFord(std::ostream & out);

private:
	class CNegativeLoopException : public std::exception
	{
	public:
		CNegativeLoopException(size_t vertexId)
			: std::exception("negative loop")
			, m_vertexId(vertexId)
		{
		}

		size_t m_vertexId;
	};

	void InitBellmanFordState();
	void PrintNegativeLoop(size_t vertexId, std::ostream & out);
	void PrintResults(std::ostream & out);
	bool UpdateBellmanFordState(bool isLastIteration);
	void GetShortestPath(size_t vertexId, std::vector<size_t> & shortestPath)const;

	std::vector<Edge> m_edges;
	std::vector<Vertex> m_verticies;
	size_t m_startId = 0;
};
