#include "stdafx.h"
#include "BoostGraph.h"


// "����������" ������� ���������, ������������ ��� ������ ��� ������������ ������.
const int64_t CBoostGraph::INFINITIVE_COST = std::numeric_limits<int64_t>::max();


CBoostGraph::CNegativeLoopException::CNegativeLoopException(size_t vertexId)
	: std::exception("negative loop")
	, m_vertexId(vertexId)
{
}

CBoostGraph::CBoostGraph()
{
}


CBoostGraph::~CBoostGraph()
{
}

bool CBoostGraph::ReadGraphviz(std::istream & in)
{
	// ���������� �������� ������� 'root' ��� ����������� ��������� �������.
	// http://www.graphviz.org/doc/info/attrs.html#d:root
	const char C_IS_START[] = "root";
	const char C_WEIGHT[] = "weight";
	try
	{
		boost::dynamic_properties dp(boost::ignore_other_properties);
		dp.property(C_IS_START, boost::get(&Vertex::isStart, m_graph));
		dp.property(C_WEIGHT, boost::get(&Edge::weight, m_graph));

		boost::read_graphviz(in, m_graph, dp);
	}
	catch (std::exception const& ex)
	{
		std::cerr << "ReadGraphviz failed: " << ex.what() << std::endl;
		return false;
	}

	// ���� ��������� �������.
	auto vertices = m_graph.vertex_set();
	auto it = std::find_if(vertices.begin(), vertices.end(), [this](size_t vi) {
		return m_graph[vi].isStart;
	});
	if (it == vertices.end())
	{
		std::cerr << "Exactly 1 vertex with [" << C_IS_START << "=true] required." << std::endl;
		return false;
	}
	m_startId = *it;

	return true;
}

void CBoostGraph::RunBellmanFord(std::ostream & out)
{
	try
	{
		InitBellmanFordState();
		auto vertices = m_graph.vertex_set();
		for (size_t vertexId : vertices)
		{
			const bool isLastIteration = (vertexId + 1 == size_t(vertices.size()));
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

void CBoostGraph::InitBellmanFordState()
{
	/* ����� ����� ��������� - ������ �������� ��� ������������� ��� ������ �������.
	vertex_iterator_t vi;
	vertex_iterator_t viEnd;
	for (std::tie(vi, viEnd) = boost::vertices(m_graph); vi != viEnd; ++vi)
	{
		m_graph[*vi].cost = INFINITIVE_COST;
	}
	*/

	// ����� ����������� (C++ 2011) ������� ������ ������:
	for (size_t vertexId : m_graph.vertex_set())
	{
		m_graph[vertexId].cost = INFINITIVE_COST;
	}
	// �������������� ��������� �������.
	{
		Vertex &start = m_graph[m_startId];
		start.cost = 0;
		start.previous = m_startId;
	}
}

void CBoostGraph::PrintNegativeLoop(size_t vertexId, std::ostream & out)
{
	// ������� ����, ���� �� ������� � vertexId.
	std::vector<size_t> negativeLoop;
	negativeLoop.push_back(vertexId);
	for (;;)
	{
		size_t previous = m_graph[negativeLoop.back()].previous;
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

void CBoostGraph::PrintResults(std::ostream & out)
{
	std::vector<size_t> shortestPath;
	for (size_t vertexId : m_graph.vertex_set())
	{
		Vertex const& vertex = m_graph[vertexId];
		if (vertex.cost == INFINITIVE_COST)
		{
			out << "No" << std::endl;
		}
		else
		{
			out << vertex.cost << ' ';
			GetShortestPath(vertexId, shortestPath);
			out << shortestPath.size();
			for (size_t vertexId : shortestPath)
			{
				out << ' ' << (vertexId + 1);
			}
			out << std::endl;
		}
	}
}

bool CBoostGraph::UpdateBellmanFordState(bool isLastIteration)
{
	/* ����� ����� ��������� - ������ �������� ��� ������������� ��� ������ �����.
	edge_iterator_t ei;
	edge_iterator_t eiEnd;
	for (std::tie(ei, eiEnd) = boost::edges(m_graph); ei != eiEnd; ++ei)
	{
		// do something.
	}
	*/

	/* ����� ����������� (C++ 2011) ������� ������ ������:
	edge_iterator_t ei;
	edge_iterator_t eiEnd;
	std::tie(ei, eiEnd) = boost::edges(m_graph);
	auto range = boost::make_iterator_range(ei, eiEnd);
	*/

	// ��������� ���������� �� �����.
	// ����, ������������ ����� ��������, ����� ��������� � ������.
	auto range = boost::make_iterator_range(boost::edges(m_graph));
	bool hasChanges = false;
	for (auto edge : range)
	{
		int64_t startWeight = m_graph[edge.m_source].cost;
		int64_t endWeight = m_graph[edge.m_target].cost;
		int64_t edgeWeight = m_graph[edge].weight;
		if ((startWeight != INFINITIVE_COST) && (endWeight > startWeight + edgeWeight))
		{
			if (isLastIteration)
			{
				// �� ��������� �������� ��������� ��� �����-�� ������� �����������.
				// ������, ���� ����������� ����.
				throw CNegativeLoopException(edge.m_target);
			}
			else
			{
				m_graph[edge.m_target].cost = startWeight + edgeWeight;
				m_graph[edge.m_target].previous = edge.m_source;
				hasChanges = true;
			}
		}
	}

	return hasChanges;
}

void CBoostGraph::GetShortestPath(size_t vertexId, std::vector<size_t>& shortestPath) const
{
	shortestPath.clear();
	for (size_t i = vertexId; i != m_startId; i = m_graph[shortestPath.back()].previous)
	{
		shortestPath.push_back(i);
	}
	std::reverse(shortestPath.begin(), shortestPath.end());
}
