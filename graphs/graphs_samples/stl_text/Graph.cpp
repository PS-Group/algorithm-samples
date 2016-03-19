#include "stdafx.h"
#include "Graph.h"


// "����������" ������� ���������, ������������ ��� ������ ��� ������������ ������.
const int64_t CGraph::INFINITIVE_COST = std::numeric_limits<int64_t>::max();


CGraph::CGraph()
{
}


CGraph::~CGraph()
{
}

void CGraph::ReadText(std::istream & in)
{
	size_t edgeCount = 0;
	size_t vertexCount = 0;
	in >> vertexCount >> edgeCount >> m_startId;
	// ������ ��������� �� �������, �.�. ���������� ������� ���������� � 0.
	--m_startId;

	m_edges.resize(edgeCount);
	m_verticies.resize(vertexCount);

	for (Edge &edge : m_edges)
	{
		in >> edge.start >> edge.end >> edge.weight;
		// ������� ��������� �� �������, �.�. ���������� ������� ���������� � 0.
		--edge.start;
		--edge.end;
	}
}

// �������� ��������-�����:
// https://en.wikipedia.org/wiki/Bellman%E2%80%93Ford_algorithm
// ��������� (��� ������ ������������� ������):
//	for v : Vertices:
//		vertex_cost(v) = +INFINITY
//  vertex_cost(v_start) = 0
//	for i : 0..|Vertices|-1:
//		for (u, v) : Edges:
//			if vertex_cost(v_i) > vertex_cost(u) + edge_weight(u->v):
//				vertex_cost(v_i)
//
// ��� ������ ������������� ������ �� ��������� ��������� ��������.
// ���� ������������� ����� ���, ��� ������ �� �����������.
void CGraph::RunBellmanFord(std::ostream & out)
{
	try
	{
		InitBellmanFordState();
		for (size_t i = 0; i < m_verticies.size(); ++i)
		{
			const bool isLastIteration = (i + 1 == m_verticies.size());
			// ��������� ��������� �������� �� ���� �����.
			// ����������� ���������: ���� ��������� �� ����������, ��������� ����.
			if (!UpdateBellmanFordState(isLastIteration))
			{
				break;
			}
		}
		PrintResults(out);
	}
	catch (CNegativeLoopException const& ex)
	{
		// ������ ���� ������������� ���������, �������� ���.
		PrintNegativeLoop(ex.m_vertexId, out);
	}
}

void CGraph::InitBellmanFordState()
{
	// ���������� ���� ����������� ���������.
	for (Vertex &v : m_verticies)
	{
		v.cost = INFINITIVE_COST;
	}
	// �������������� ��������� �������.
	{
		Vertex &start = m_verticies[m_startId];
		start.cost = 0;
		start.previous = m_startId;
	}
}

void CGraph::PrintNegativeLoop(size_t vertexId, std::ostream & out)
{
	// ������� ����, ���� �� ������� � vertexId.
	std::vector<size_t> negativeLoop;
	negativeLoop.push_back(vertexId);
	for (;;)
	{
		size_t previous = m_verticies[negativeLoop.back()].previous;
		negativeLoop.push_back(previous);
		if (previous == vertexId)
		{
			break;
		}
	}
	// ������ ������� �� ��������, ����� ����������� ���� � ������.
	std::reverse(negativeLoop.begin(), negativeLoop.end());

	out << "No" << std::endl;
	out << negativeLoop.size();
	for (size_t vertexId : negativeLoop)
	{
		out << ' ' << (vertexId + 1);
	}
	out << std::endl;
}

void CGraph::PrintResults(std::ostream & out)
{
	std::vector<size_t> shortestPath;
	for (size_t i = 0; i < m_verticies.size(); ++i)
	{
		Vertex const& vertex = m_verticies[i];
		if (vertex.cost == INFINITIVE_COST)
		{
			out << "No" << std::endl;
		}
		else
		{
			out << vertex.cost << ' ';
			GetShortestPath(i, shortestPath);
			out << shortestPath.size();
			for (size_t vertexId : shortestPath)
			{
				out << ' ' << (vertexId + 1);
			}
			out << std::endl;
		}
	}
}

void CGraph::GetShortestPath(size_t vertexId, std::vector<size_t> & shortestPath)const
{
	shortestPath.clear();
	for (size_t i = vertexId; i != m_startId; i = m_verticies[shortestPath.back()].previous)
	{
		shortestPath.push_back(i);
	}
	std::reverse(shortestPath.begin(), shortestPath.end());
}

bool CGraph::UpdateBellmanFordState(bool isLastIteration)
{
	bool hasChanges = false;
	for (Edge const& edge : m_edges)
	{
		int64_t startWeight = m_verticies[edge.start].cost;
		int64_t endWeight = m_verticies[edge.end].cost;
		int64_t edgeWeight = edge.weight;
		if ((startWeight != INFINITIVE_COST) && (endWeight > startWeight + edgeWeight))
		{
			if (isLastIteration)
			{
				// �� ��������� �������� ��������� ��� �����-�� ������� �����������.
				// ������, ���� ����������� ����.
				throw CNegativeLoopException(edge.end);
			}
			else
			{
				m_verticies[edge.end].cost = startWeight + edgeWeight;
				m_verticies[edge.end].previous = edge.start;
				hasChanges = true;
			}
		}
	}
	return hasChanges;
}
