/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   uploadedFile.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: arcebria <arcebria@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/01 17:51:48 by arcebria          #+#    #+#             */
/*   Updated: 2025/07/07 19:50:59 by arcebria         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UPLOADEDFILE_HPP
#define UPLOADEDFILE_HPP

#include <string>
#include <vector>
#include <fstream>
#include "../include/utils.hpp"

class	ServerResponse;
class	ClientRequest;

struct UploadedFile {
	std::string _filename;
	std::string _fieldName;
	std::string _contentType;
	std::vector<char> _content;
	size_t _size;

	UploadedFile() : _size(0) {}
};

#endif
